/*M ///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                           License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2013, OpenCV Foundation, all rights reserved.
// Copyright (C) 2017, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#include "../precomp.hpp"
#include <opencv2/dnn/shape_utils.hpp>
#include <opencv2/dnn/all_layers.hpp>
#include "../nms.inl.hpp"

#ifdef HAVE_OPENCL
#include "opencl_kernels_dnn.hpp"
#endif

namespace cv
{
namespace dnn
{

class RegionLayerImpl CV_FINAL : public RegionLayer
{
public:
    int coords, classes, anchors, classfix;
    float thresh, nmsThreshold;
    bool useSoftmax, useLogistic;

    RegionLayerImpl(const LayerParams& params)
    {
        setParamsFrom(params);
        CV_Assert(blobs.size() == 1);

        thresh = params.get<float>("thresh", 0.2);
        coords = params.get<int>("coords", 4);
        classes = params.get<int>("classes", 0);
        anchors = params.get<int>("anchors", 5);
        classfix = params.get<int>("classfix", 0);
        useSoftmax = params.get<bool>("softmax", false);
        useLogistic = params.get<bool>("logistic", false);
        nmsThreshold = params.get<float>("nms_threshold", 0.4);

        CV_Assert(nmsThreshold >= 0.);
        CV_Assert(coords == 4);
        CV_Assert(classes >= 1);
        CV_Assert(anchors >= 1);
        CV_Assert(useLogistic || useSoftmax);
        if (params.get<bool>("softmax_tree", false))
            CV_Error(cv::Error::StsNotImplemented, "Yolo9000 is not implemented");
    }

    bool getMemoryShapes(const std::vector<MatShape> &inputs,
                         const int requiredOutputs,
                         std::vector<MatShape> &outputs,
                         std::vector<MatShape> &internals) const CV_OVERRIDE
    {
        CV_Assert(inputs.size() > 0);
        CV_Assert(inputs[0][3] == (1 + coords + classes)*anchors);
        outputs = std::vector<MatShape>(1, shape(inputs[0][1] * inputs[0][2] * anchors, inputs[0][3] / anchors));
        return false;
    }

    float logistic_activate(float x) { return 1.F / (1.F + exp(-x)); }

    void softmax_activate(const float* input, const int n, const float temp, float* output)
    {
        int i;
        float sum = 0;
        float largest = -FLT_MAX;
        for (i = 0; i < n; ++i) {
            if (input[i] > largest) largest = input[i];
        }
        for (i = 0; i < n; ++i) {
            float e = exp((input[i] - largest) / temp);
            sum += e;
            output[i] = e;
        }
        for (i = 0; i < n; ++i) {
            output[i] /= sum;
        }
    }

#ifdef HAVE_OPENCL
    bool forward_ocl(InputArrayOfArrays inps, OutputArrayOfArrays outs, OutputArrayOfArrays internals)
    {
        std::vector<UMat> inputs;
        std::vector<UMat> outputs;

        // TODO: implement a logistic activation to classification scores.
        if (useLogistic || inps.depth() == CV_16S)
            return false;

        inps.getUMatVector(inputs);
        outs.getUMatVector(outputs);

        CV_Assert(inputs.size() >= 1);
        int const cell_size = classes + coords + 1;
        UMat blob_umat = blobs[0].getUMat(ACCESS_READ);

        for (size_t ii = 0; ii < outputs.size(); ii++)
        {
            UMat& inpBlob = inputs[ii];
            UMat& outBlob = outputs[ii];

            int rows = inpBlob.size[1];
            int cols = inpBlob.size[2];

            ocl::Kernel logistic_kernel("logistic_activ", ocl::dnn::region_oclsrc);
            size_t global = rows*cols*anchors;
            logistic_kernel.set(0, (int)global);
            logistic_kernel.set(1, ocl::KernelArg::PtrReadOnly(inpBlob));
            logistic_kernel.set(2, (int)cell_size);
            logistic_kernel.set(3, ocl::KernelArg::PtrWriteOnly(outBlob));
            logistic_kernel.run(1, &global, NULL, false);

            if (useSoftmax)
            {
                // Yolo v2
                // softmax activation for Probability, for each grid cell (X x Y x Anchor-index)
                ocl::Kernel softmax_kernel("softmax_activ", ocl::dnn::region_oclsrc);
                size_t nthreads = rows*cols*anchors;
                softmax_kernel.set(0, (int)nthreads);
                softmax_kernel.set(1, ocl::KernelArg::PtrReadOnly(inpBlob));
                softmax_kernel.set(2, ocl::KernelArg::PtrReadOnly(blob_umat));
                softmax_kernel.set(3, (int)cell_size);
                softmax_kernel.set(4, (int)classes);
                softmax_kernel.set(5, (int)classfix);
                softmax_kernel.set(6, (int)rows);
                softmax_kernel.set(7, (int)cols);
                softmax_kernel.set(8, (int)anchors);
                softmax_kernel.set(9, (float)thresh);
                softmax_kernel.set(10, ocl::KernelArg::PtrWriteOnly(outBlob));
                if (!softmax_kernel.run(1, &nthreads, NULL, false))
                    return false;
            }

            if (nmsThreshold > 0) {
                Mat mat = outBlob.getMat(ACCESS_WRITE);
                float *dstData = mat.ptr<float>();
                do_nms_sort(dstData, rows*cols*anchors, thresh, nmsThreshold);
            }

        }

        return true;
    }
#endif

    void forward(InputArrayOfArrays inputs_arr, OutputArrayOfArrays outputs_arr, OutputArrayOfArrays internals_arr) CV_OVERRIDE
    {
        CV_TRACE_FUNCTION();
        CV_TRACE_ARG_VALUE(name, "name", name.c_str());

        CV_OCL_RUN(IS_DNN_OPENCL_TARGET(preferableTarget) &&
                   OCL_PERFORMANCE_CHECK(ocl::Device::getDefault().isIntel()),
                   forward_ocl(inputs_arr, outputs_arr, internals_arr))

        Layer::forward_fallback(inputs_arr, outputs_arr, internals_arr);
    }

    void forward(std::vector<Mat*> &inputs, std::vector<Mat> &outputs, std::vector<Mat> &internals) CV_OVERRIDE
    {
        CV_TRACE_FUNCTION();
        CV_TRACE_ARG_VALUE(name, "name", name.c_str());

        CV_Assert(inputs.size() >= 1);
        CV_Assert(outputs.size() == 1);
        int const cell_size = classes + coords + 1;

        const float* biasData = blobs[0].ptr<float>();

        for (size_t ii = 0; ii < outputs.size(); ii++)
        {
            Mat &inpBlob = *inputs[ii];
            Mat &outBlob = outputs[ii];

            int rows = inpBlob.size[1];
            int cols = inpBlob.size[2];
            CV_Assert(inputs.size() < 2 || inputs[1]->dims == 4);
            int hNorm = inputs.size() > 1 ? inputs[1]->size[2] : rows;
            int wNorm = inputs.size() > 1 ? inputs[1]->size[3] : cols;

            const float *srcData = inpBlob.ptr<float>();
            float *dstData = outBlob.ptr<float>();

            // logistic activation for t0, for each grid cell (X x Y x Anchor-index)
            for (int i = 0; i < rows*cols*anchors; ++i) {
                int index = cell_size*i;
                float x = srcData[index + 4];
                dstData[index + 4] = logistic_activate(x);	// logistic activation
            }

            if (useSoftmax) {  // Yolo v2
                // softmax activation for Probability, for each grid cell (X x Y x Anchor-index)
                for (int i = 0; i < rows*cols*anchors; ++i) {
                    int index = cell_size*i;
                    softmax_activate(srcData + index + 5, classes, 1, dstData + index + 5);
                }
            }
            else if (useLogistic) {  // Yolo v3
                for (int i = 0; i < rows*cols*anchors; ++i)
                {
                    int index = cell_size*i;
                    const float* input = srcData + index + 5;
                    float* output = dstData + index + 5;
                    for (int i = 0; i < classes; ++i)
                        output[i] = logistic_activate(input[i]);
                }
            }
            for (int x = 0; x < cols; ++x)
                for(int y = 0; y < rows; ++y)
                    for (int a = 0; a < anchors; ++a) {
                        int index = (y*cols + x)*anchors + a;  // index for each grid-cell & anchor
                        int p_index = index * cell_size + 4;
                        float scale = dstData[p_index];
                        if (classfix == -1 && scale < .5) scale = 0;  // if(t0 < 0.5) t0 = 0;
                        int box_index = index * cell_size;

                        dstData[box_index + 0] = (x + logistic_activate(srcData[box_index + 0])) / cols;
                        dstData[box_index + 1] = (y + logistic_activate(srcData[box_index + 1])) / rows;
                        dstData[box_index + 2] = exp(srcData[box_index + 2]) * biasData[2 * a] / hNorm;
                        dstData[box_index + 3] = exp(srcData[box_index + 3]) * biasData[2 * a + 1] / wNorm;

                        int class_index = index * cell_size + 5;

                        for (int j = 0; j < classes; ++j) {
                            float prob = scale*dstData[class_index + j];  // prob = IoU(box, object) = t0 * class-probability
                            dstData[class_index + j] = (prob > thresh) ? prob : 0;  // if (IoU < threshold) IoU = 0;
                        }
                    }
            if (nmsThreshold > 0) {
                do_nms_sort(dstData, rows*cols*anchors, thresh, nmsThreshold);
            }
        }
    }

    static inline float rectOverlap(const Rect2f& a, const Rect2f& b)
    {
        return 1.0f - jaccardDistance(a, b);
    }

    void do_nms_sort(float *detections, int total, float score_thresh, float nms_thresh)
    {
        std::vector<Rect2f> boxes(total);
        std::vector<float> scores(total);

        for (int i = 0; i < total; ++i)
        {
            Rect2f &b = boxes[i];
            int box_index = i * (classes + coords + 1);
            b.width = detections[box_index + 2];
            b.height = detections[box_index + 3];
            b.x = detections[box_index + 0] - b.width / 2;
            b.y = detections[box_index + 1] - b.height / 2;
        }

        std::vector<int> indices;
        for (int k = 0; k < classes; ++k)
        {
            for (int i = 0; i < total; ++i)
            {
                int box_index = i * (classes + coords + 1);
                int class_index = box_index + 5;
                scores[i] = detections[class_index + k];
                detections[class_index + k] = 0;
            }
            NMSFast_(boxes, scores, score_thresh, nms_thresh, 1, 0, indices, rectOverlap);
            for (int i = 0, n = indices.size(); i < n; ++i)
            {
                int box_index = indices[i] * (classes + coords + 1);
                int class_index = box_index + 5;
                detections[class_index + k] = scores[indices[i]];
            }
        }
    }

    virtual int64 getFLOPS(const std::vector<MatShape> &inputs,
                           const std::vector<MatShape> &outputs) const CV_OVERRIDE
    {
        (void)outputs; // suppress unused variable warning

        int64 flops = 0;
        for(int i = 0; i < inputs.size(); i++)
        {
            flops += 60*total(inputs[i]);
        }
        return flops;
    }
};

Ptr<RegionLayer> RegionLayer::create(const LayerParams& params)
{
    return Ptr<RegionLayer>(new RegionLayerImpl(params));
}

}  // namespace dnn
}  // namespace cv
