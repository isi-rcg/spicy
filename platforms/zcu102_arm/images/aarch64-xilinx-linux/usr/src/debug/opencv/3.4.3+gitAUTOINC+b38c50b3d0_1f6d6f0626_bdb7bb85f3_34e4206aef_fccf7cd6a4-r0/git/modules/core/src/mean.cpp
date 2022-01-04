// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html


#include "precomp.hpp"
#include "opencl_kernels_core.hpp"
#include "opencv2/core/openvx/ovx_defs.hpp"
#include "stat.hpp"

#if defined HAVE_IPP
namespace cv
{
static bool ipp_mean( Mat &src, Mat &mask, Scalar &ret )
{
    CV_INSTRUMENT_REGION_IPP()

#if IPP_VERSION_X100 >= 700
    size_t total_size = src.total();
    int cn = src.channels();
    if (cn > 4)
        return false;
    int rows = src.size[0], cols = rows ? (int)(total_size/rows) : 0;
    if( src.dims == 2 || (src.isContinuous() && mask.isContinuous() && cols > 0 && (size_t)rows*cols == total_size) )
    {
        IppiSize sz = { cols, rows };
        int type = src.type();
        if( !mask.empty() )
        {
            typedef IppStatus (CV_STDCALL* ippiMaskMeanFuncC1)(const void *, int, const void *, int, IppiSize, Ipp64f *);
            ippiMaskMeanFuncC1 ippiMean_C1MR =
            type == CV_8UC1 ? (ippiMaskMeanFuncC1)ippiMean_8u_C1MR :
            type == CV_16UC1 ? (ippiMaskMeanFuncC1)ippiMean_16u_C1MR :
            type == CV_32FC1 ? (ippiMaskMeanFuncC1)ippiMean_32f_C1MR :
            0;
            if( ippiMean_C1MR )
            {
                Ipp64f res;
                if( CV_INSTRUMENT_FUN_IPP(ippiMean_C1MR, src.ptr(), (int)src.step[0], mask.ptr(), (int)mask.step[0], sz, &res) >= 0 )
                {
                    ret = Scalar(res);
                    return true;
                }
            }
            typedef IppStatus (CV_STDCALL* ippiMaskMeanFuncC3)(const void *, int, const void *, int, IppiSize, int, Ipp64f *);
            ippiMaskMeanFuncC3 ippiMean_C3MR =
            type == CV_8UC3 ? (ippiMaskMeanFuncC3)ippiMean_8u_C3CMR :
            type == CV_16UC3 ? (ippiMaskMeanFuncC3)ippiMean_16u_C3CMR :
            type == CV_32FC3 ? (ippiMaskMeanFuncC3)ippiMean_32f_C3CMR :
            0;
            if( ippiMean_C3MR )
            {
                Ipp64f res1, res2, res3;
                if( CV_INSTRUMENT_FUN_IPP(ippiMean_C3MR, src.ptr(), (int)src.step[0], mask.ptr(), (int)mask.step[0], sz, 1, &res1) >= 0 &&
                    CV_INSTRUMENT_FUN_IPP(ippiMean_C3MR, src.ptr(), (int)src.step[0], mask.ptr(), (int)mask.step[0], sz, 2, &res2) >= 0 &&
                    CV_INSTRUMENT_FUN_IPP(ippiMean_C3MR, src.ptr(), (int)src.step[0], mask.ptr(), (int)mask.step[0], sz, 3, &res3) >= 0 )
                {
                    ret = Scalar(res1, res2, res3);
                    return true;
                }
            }
        }
        else
        {
            typedef IppStatus (CV_STDCALL* ippiMeanFuncHint)(const void*, int, IppiSize, double *, IppHintAlgorithm);
            typedef IppStatus (CV_STDCALL* ippiMeanFuncNoHint)(const void*, int, IppiSize, double *);
            ippiMeanFuncHint ippiMeanHint =
                type == CV_32FC1 ? (ippiMeanFuncHint)ippiMean_32f_C1R :
                type == CV_32FC3 ? (ippiMeanFuncHint)ippiMean_32f_C3R :
                type == CV_32FC4 ? (ippiMeanFuncHint)ippiMean_32f_C4R :
                0;
            ippiMeanFuncNoHint ippiMean =
                type == CV_8UC1 ? (ippiMeanFuncNoHint)ippiMean_8u_C1R :
                type == CV_8UC3 ? (ippiMeanFuncNoHint)ippiMean_8u_C3R :
                type == CV_8UC4 ? (ippiMeanFuncNoHint)ippiMean_8u_C4R :
                type == CV_16UC1 ? (ippiMeanFuncNoHint)ippiMean_16u_C1R :
                type == CV_16UC3 ? (ippiMeanFuncNoHint)ippiMean_16u_C3R :
                type == CV_16UC4 ? (ippiMeanFuncNoHint)ippiMean_16u_C4R :
                type == CV_16SC1 ? (ippiMeanFuncNoHint)ippiMean_16s_C1R :
                type == CV_16SC3 ? (ippiMeanFuncNoHint)ippiMean_16s_C3R :
                type == CV_16SC4 ? (ippiMeanFuncNoHint)ippiMean_16s_C4R :
                0;
            // Make sure only zero or one version of the function pointer is valid
            CV_Assert(!ippiMeanHint || !ippiMean);
            if( ippiMeanHint || ippiMean )
            {
                Ipp64f res[4];
                IppStatus status = ippiMeanHint ? CV_INSTRUMENT_FUN_IPP(ippiMeanHint, src.ptr(), (int)src.step[0], sz, res, ippAlgHintAccurate) :
                                CV_INSTRUMENT_FUN_IPP(ippiMean, src.ptr(), (int)src.step[0], sz, res);
                if( status >= 0 )
                {
                    for( int i = 0; i < cn; i++ )
                        ret[i] = res[i];
                    return true;
                }
            }
        }
    }
    return false;
#else
    return false;
#endif
}
}
#endif

cv::Scalar cv::mean( InputArray _src, InputArray _mask )
{
    CV_INSTRUMENT_REGION()

    Mat src = _src.getMat(), mask = _mask.getMat();
    CV_Assert( mask.empty() || mask.type() == CV_8U );

    int k, cn = src.channels(), depth = src.depth();
    Scalar s;

    CV_IPP_RUN(IPP_VERSION_X100 >= 700, ipp_mean(src, mask, s), s)

    SumFunc func = getSumFunc(depth);

    CV_Assert( cn <= 4 && func != 0 );

    const Mat* arrays[] = {&src, &mask, 0};
    uchar* ptrs[2];
    NAryMatIterator it(arrays, ptrs);
    int total = (int)it.size, blockSize = total, intSumBlockSize = 0;
    int j, count = 0;
    AutoBuffer<int> _buf;
    int* buf = (int*)&s[0];
    bool blockSum = depth <= CV_16S;
    size_t esz = 0, nz0 = 0;

    if( blockSum )
    {
        intSumBlockSize = depth <= CV_8S ? (1 << 23) : (1 << 15);
        blockSize = std::min(blockSize, intSumBlockSize);
        _buf.allocate(cn);
        buf = _buf.data();

        for( k = 0; k < cn; k++ )
            buf[k] = 0;
        esz = src.elemSize();
    }

    for( size_t i = 0; i < it.nplanes; i++, ++it )
    {
        for( j = 0; j < total; j += blockSize )
        {
            int bsz = std::min(total - j, blockSize);
            int nz = func( ptrs[0], ptrs[1], (uchar*)buf, bsz, cn );
            count += nz;
            nz0 += nz;
            if( blockSum && (count + blockSize >= intSumBlockSize || (i+1 >= it.nplanes && j+bsz >= total)) )
            {
                for( k = 0; k < cn; k++ )
                {
                    s[k] += buf[k];
                    buf[k] = 0;
                }
                count = 0;
            }
            ptrs[0] += bsz*esz;
            if( ptrs[1] )
                ptrs[1] += bsz;
        }
    }
    return s*(nz0 ? 1./nz0 : 0);
}

//==================================================================================================

namespace cv {

template <typename T, typename ST, typename SQT>
struct SumSqr_SIMD
{
    int operator () (const T *, const uchar *, ST *, SQT *, int, int) const
    {
        return 0;
    }
};

template <typename T>
inline void addSqrChannels(T * sum, T * sqsum, T * buf, int cn)
{
    for (int i = 0; i < 4; ++i)
    {
        sum[i % cn] += buf[i];
        sqsum[i % cn] += buf[4 + i];
    }
}

#if CV_SSE2

template <>
struct SumSqr_SIMD<uchar, int, int>
{
    int operator () (const uchar * src0, const uchar * mask, int * sum, int * sqsum, int len, int cn) const
    {
        if (mask || (cn != 1 && cn != 2) || !USE_SSE2)
            return 0;

        int x = 0;
        __m128i v_zero = _mm_setzero_si128(), v_sum = v_zero, v_sqsum = v_zero;
        const int len_16 = len & ~15;

        for ( ; x <= len_16 - 16; )
        {
            const int len_tmp = min(x + 2048, len_16);
            __m128i v_sum_tmp = v_zero;
            for ( ; x <= len_tmp - 16; x += 16)
            {
                __m128i v_src = _mm_loadu_si128((const __m128i *)(src0 + x));
                __m128i v_half_0 = _mm_unpacklo_epi8(v_src, v_zero);
                __m128i v_half_1 = _mm_unpackhi_epi8(v_src, v_zero);
                v_sum_tmp = _mm_add_epi16(v_sum_tmp, _mm_add_epi16(v_half_0, v_half_1));
                __m128i v_half_2 = _mm_unpacklo_epi16(v_half_0, v_half_1);
                __m128i v_half_3 = _mm_unpackhi_epi16(v_half_0, v_half_1);
                v_sqsum = _mm_add_epi32(v_sqsum, _mm_madd_epi16(v_half_2, v_half_2));
                v_sqsum = _mm_add_epi32(v_sqsum, _mm_madd_epi16(v_half_3, v_half_3));
            }
            v_sum = _mm_add_epi32(v_sum, _mm_unpacklo_epi16(v_sum_tmp, v_zero));
            v_sum = _mm_add_epi32(v_sum, _mm_unpackhi_epi16(v_sum_tmp, v_zero));
        }

        for ( ; x <= len - 8; x += 8)
        {
            __m128i v_src = _mm_unpacklo_epi8(_mm_loadl_epi64((__m128i const *)(src0 + x)), v_zero);
            __m128i v_half_0 = _mm_unpackhi_epi64(v_src, v_src);
            __m128i v_sum_tmp = _mm_add_epi16(v_src, v_half_0);
            __m128i v_half_1 = _mm_unpacklo_epi16(v_src, v_half_0);

            v_sum = _mm_add_epi32(v_sum, _mm_unpacklo_epi16(v_sum_tmp, v_zero));
            v_sqsum = _mm_add_epi32(v_sqsum, _mm_madd_epi16(v_half_1, v_half_1));
        }

        int CV_DECL_ALIGNED(16) ar[8];
        _mm_store_si128((__m128i*)ar, v_sum);
        _mm_store_si128((__m128i*)(ar + 4), v_sqsum);

        addSqrChannels(sum, sqsum, ar, cn);

        return x / cn;
    }
};

template <>
struct SumSqr_SIMD<schar, int, int>
{
    int operator () (const schar * src0, const uchar * mask, int * sum, int * sqsum, int len, int cn) const
    {
        if (mask || (cn != 1 && cn != 2) || !USE_SSE2)
            return 0;

        int x = 0;
        __m128i v_zero = _mm_setzero_si128(), v_sum = v_zero, v_sqsum = v_zero;
        const int len_16 = len & ~15;

        for ( ; x <= len_16 - 16; )
        {
            const int len_tmp = min(x + 2048, len_16);
            __m128i v_sum_tmp = v_zero;
            for ( ; x <= len_tmp - 16; x += 16)
            {
                __m128i v_src = _mm_loadu_si128((const __m128i *)(src0 + x));
                __m128i v_half_0 = _mm_srai_epi16(_mm_unpacklo_epi8(v_zero, v_src), 8);
                __m128i v_half_1 = _mm_srai_epi16(_mm_unpackhi_epi8(v_zero, v_src), 8);
                v_sum_tmp = _mm_add_epi16(v_sum_tmp, _mm_add_epi16(v_half_0, v_half_1));
                __m128i v_half_2 = _mm_unpacklo_epi16(v_half_0, v_half_1);
                __m128i v_half_3 = _mm_unpackhi_epi16(v_half_0, v_half_1);
                v_sqsum = _mm_add_epi32(v_sqsum, _mm_madd_epi16(v_half_2, v_half_2));
                v_sqsum = _mm_add_epi32(v_sqsum, _mm_madd_epi16(v_half_3, v_half_3));
            }
            v_sum = _mm_add_epi32(v_sum, _mm_srai_epi32(_mm_unpacklo_epi16(v_zero, v_sum_tmp), 16));
            v_sum = _mm_add_epi32(v_sum, _mm_srai_epi32(_mm_unpackhi_epi16(v_zero, v_sum_tmp), 16));
        }

        for ( ; x <= len - 8; x += 8)
        {
            __m128i v_src = _mm_srai_epi16(_mm_unpacklo_epi8(v_zero, _mm_loadl_epi64((__m128i const *)(src0 + x))), 8);
            __m128i v_half_0 = _mm_unpackhi_epi64(v_src, v_src);
            __m128i v_sum_tmp = _mm_add_epi16(v_src, v_half_0);
            __m128i v_half_1 = _mm_unpacklo_epi16(v_src, v_half_0);

            v_sum = _mm_add_epi32(v_sum, _mm_srai_epi32(_mm_unpacklo_epi16(v_zero, v_sum_tmp), 16));
            v_sqsum = _mm_add_epi32(v_sqsum, _mm_madd_epi16(v_half_1, v_half_1));
        }

        int CV_DECL_ALIGNED(16) ar[8];
        _mm_store_si128((__m128i*)ar, v_sum);
        _mm_store_si128((__m128i*)(ar + 4), v_sqsum);

        addSqrChannels(sum, sqsum, ar, cn);

        return x / cn;
    }
};

#endif

template<typename T, typename ST, typename SQT>
static int sumsqr_(const T* src0, const uchar* mask, ST* sum, SQT* sqsum, int len, int cn )
{
    const T* src = src0;

    if( !mask )
    {
        SumSqr_SIMD<T, ST, SQT> vop;
        int i = vop(src0, mask, sum, sqsum, len, cn), k = cn % 4;
        src += i * cn;

        if( k == 1 )
        {
            ST s0 = sum[0];
            SQT sq0 = sqsum[0];
            for( ; i < len; i++, src += cn )
            {
                T v = src[0];
                s0 += v; sq0 += (SQT)v*v;
            }
            sum[0] = s0;
            sqsum[0] = sq0;
        }
        else if( k == 2 )
        {
            ST s0 = sum[0], s1 = sum[1];
            SQT sq0 = sqsum[0], sq1 = sqsum[1];
            for( ; i < len; i++, src += cn )
            {
                T v0 = src[0], v1 = src[1];
                s0 += v0; sq0 += (SQT)v0*v0;
                s1 += v1; sq1 += (SQT)v1*v1;
            }
            sum[0] = s0; sum[1] = s1;
            sqsum[0] = sq0; sqsum[1] = sq1;
        }
        else if( k == 3 )
        {
            ST s0 = sum[0], s1 = sum[1], s2 = sum[2];
            SQT sq0 = sqsum[0], sq1 = sqsum[1], sq2 = sqsum[2];
            for( ; i < len; i++, src += cn )
            {
                T v0 = src[0], v1 = src[1], v2 = src[2];
                s0 += v0; sq0 += (SQT)v0*v0;
                s1 += v1; sq1 += (SQT)v1*v1;
                s2 += v2; sq2 += (SQT)v2*v2;
            }
            sum[0] = s0; sum[1] = s1; sum[2] = s2;
            sqsum[0] = sq0; sqsum[1] = sq1; sqsum[2] = sq2;
        }

        for( ; k < cn; k += 4 )
        {
            src = src0 + k;
            ST s0 = sum[k], s1 = sum[k+1], s2 = sum[k+2], s3 = sum[k+3];
            SQT sq0 = sqsum[k], sq1 = sqsum[k+1], sq2 = sqsum[k+2], sq3 = sqsum[k+3];
            for( ; i < len; i++, src += cn )
            {
                T v0, v1;
                v0 = src[0], v1 = src[1];
                s0 += v0; sq0 += (SQT)v0*v0;
                s1 += v1; sq1 += (SQT)v1*v1;
                v0 = src[2], v1 = src[3];
                s2 += v0; sq2 += (SQT)v0*v0;
                s3 += v1; sq3 += (SQT)v1*v1;
            }
            sum[k] = s0; sum[k+1] = s1;
            sum[k+2] = s2; sum[k+3] = s3;
            sqsum[k] = sq0; sqsum[k+1] = sq1;
            sqsum[k+2] = sq2; sqsum[k+3] = sq3;
        }
        return len;
    }

    int i, nzm = 0;

    if( cn == 1 )
    {
        ST s0 = sum[0];
        SQT sq0 = sqsum[0];
        for( i = 0; i < len; i++ )
            if( mask[i] )
            {
                T v = src[i];
                s0 += v; sq0 += (SQT)v*v;
                nzm++;
            }
        sum[0] = s0;
        sqsum[0] = sq0;
    }
    else if( cn == 3 )
    {
        ST s0 = sum[0], s1 = sum[1], s2 = sum[2];
        SQT sq0 = sqsum[0], sq1 = sqsum[1], sq2 = sqsum[2];
        for( i = 0; i < len; i++, src += 3 )
            if( mask[i] )
            {
                T v0 = src[0], v1 = src[1], v2 = src[2];
                s0 += v0; sq0 += (SQT)v0*v0;
                s1 += v1; sq1 += (SQT)v1*v1;
                s2 += v2; sq2 += (SQT)v2*v2;
                nzm++;
            }
        sum[0] = s0; sum[1] = s1; sum[2] = s2;
        sqsum[0] = sq0; sqsum[1] = sq1; sqsum[2] = sq2;
    }
    else
    {
        for( i = 0; i < len; i++, src += cn )
            if( mask[i] )
            {
                for( int k = 0; k < cn; k++ )
                {
                    T v = src[k];
                    ST s = sum[k] + v;
                    SQT sq = sqsum[k] + (SQT)v*v;
                    sum[k] = s; sqsum[k] = sq;
                }
                nzm++;
            }
    }
    return nzm;
}


static int sqsum8u( const uchar* src, const uchar* mask, int* sum, int* sqsum, int len, int cn )
{ return sumsqr_(src, mask, sum, sqsum, len, cn); }

static int sqsum8s( const schar* src, const uchar* mask, int* sum, int* sqsum, int len, int cn )
{ return sumsqr_(src, mask, sum, sqsum, len, cn); }

static int sqsum16u( const ushort* src, const uchar* mask, int* sum, double* sqsum, int len, int cn )
{ return sumsqr_(src, mask, sum, sqsum, len, cn); }

static int sqsum16s( const short* src, const uchar* mask, int* sum, double* sqsum, int len, int cn )
{ return sumsqr_(src, mask, sum, sqsum, len, cn); }

static int sqsum32s( const int* src, const uchar* mask, double* sum, double* sqsum, int len, int cn )
{ return sumsqr_(src, mask, sum, sqsum, len, cn); }

static int sqsum32f( const float* src, const uchar* mask, double* sum, double* sqsum, int len, int cn )
{ return sumsqr_(src, mask, sum, sqsum, len, cn); }

static int sqsum64f( const double* src, const uchar* mask, double* sum, double* sqsum, int len, int cn )
{ return sumsqr_(src, mask, sum, sqsum, len, cn); }

typedef int (*SumSqrFunc)(const uchar*, const uchar* mask, uchar*, uchar*, int, int);

static SumSqrFunc getSumSqrTab(int depth)
{
    static SumSqrFunc sumSqrTab[] =
    {
        (SumSqrFunc)GET_OPTIMIZED(sqsum8u), (SumSqrFunc)sqsum8s, (SumSqrFunc)sqsum16u, (SumSqrFunc)sqsum16s,
        (SumSqrFunc)sqsum32s, (SumSqrFunc)GET_OPTIMIZED(sqsum32f), (SumSqrFunc)sqsum64f, 0
    };

    return sumSqrTab[depth];
}

#ifdef HAVE_OPENCL
static bool ocl_meanStdDev( InputArray _src, OutputArray _mean, OutputArray _sdv, InputArray _mask )
{
    CV_INSTRUMENT_REGION_OPENCL()

    bool haveMask = _mask.kind() != _InputArray::NONE;
    int nz = haveMask ? -1 : (int)_src.total();
    Scalar mean(0), stddev(0);
    const int cn = _src.channels();
    if (cn > 4)
        return false;

    {
        int type = _src.type(), depth = CV_MAT_DEPTH(type);
        bool doubleSupport = ocl::Device::getDefault().doubleFPConfig() > 0,
                isContinuous = _src.isContinuous(),
                isMaskContinuous = _mask.isContinuous();
        const ocl::Device &defDev = ocl::Device::getDefault();
        int groups = defDev.maxComputeUnits();
        if (defDev.isIntel())
        {
            static const int subSliceEUCount = 10;
            groups = (groups / subSliceEUCount) * 2;
        }
        size_t wgs = defDev.maxWorkGroupSize();

        int ddepth = std::max(CV_32S, depth), sqddepth = std::max(CV_32F, depth),
                dtype = CV_MAKE_TYPE(ddepth, cn),
                sqdtype = CV_MAKETYPE(sqddepth, cn);
        CV_Assert(!haveMask || _mask.type() == CV_8UC1);

        int wgs2_aligned = 1;
        while (wgs2_aligned < (int)wgs)
            wgs2_aligned <<= 1;
        wgs2_aligned >>= 1;

        if ( (!doubleSupport && depth == CV_64F) )
            return false;

        char cvt[2][40];
        String opts = format("-D srcT=%s -D srcT1=%s -D dstT=%s -D dstT1=%s -D sqddepth=%d"
                             " -D sqdstT=%s -D sqdstT1=%s -D convertToSDT=%s -D cn=%d%s%s"
                             " -D convertToDT=%s -D WGS=%d -D WGS2_ALIGNED=%d%s%s",
                             ocl::typeToStr(type), ocl::typeToStr(depth),
                             ocl::typeToStr(dtype), ocl::typeToStr(ddepth), sqddepth,
                             ocl::typeToStr(sqdtype), ocl::typeToStr(sqddepth),
                             ocl::convertTypeStr(depth, sqddepth, cn, cvt[0]),
                             cn, isContinuous ? " -D HAVE_SRC_CONT" : "",
                             isMaskContinuous ? " -D HAVE_MASK_CONT" : "",
                             ocl::convertTypeStr(depth, ddepth, cn, cvt[1]),
                             (int)wgs, wgs2_aligned, haveMask ? " -D HAVE_MASK" : "",
                             doubleSupport ? " -D DOUBLE_SUPPORT" : "");

        ocl::Kernel k("meanStdDev", ocl::core::meanstddev_oclsrc, opts);
        if (k.empty())
            return false;

        int dbsize = groups * ((haveMask ? CV_ELEM_SIZE1(CV_32S) : 0) +
                               CV_ELEM_SIZE(sqdtype) + CV_ELEM_SIZE(dtype));
        UMat src = _src.getUMat(), db(1, dbsize, CV_8UC1), mask = _mask.getUMat();

        ocl::KernelArg srcarg = ocl::KernelArg::ReadOnlyNoSize(src),
                dbarg = ocl::KernelArg::PtrWriteOnly(db),
                maskarg = ocl::KernelArg::ReadOnlyNoSize(mask);

        if (haveMask)
            k.args(srcarg, src.cols, (int)src.total(), groups, dbarg, maskarg);
        else
            k.args(srcarg, src.cols, (int)src.total(), groups, dbarg);

        size_t globalsize = groups * wgs;

        if(!k.run(1, &globalsize, &wgs, false))
            return false;

        typedef Scalar (* part_sum)(Mat m);
        part_sum funcs[3] = { ocl_part_sum<int>, ocl_part_sum<float>, ocl_part_sum<double> };
        Mat dbm = db.getMat(ACCESS_READ);

        mean = funcs[ddepth - CV_32S](Mat(1, groups, dtype, dbm.ptr()));
        stddev = funcs[sqddepth - CV_32S](Mat(1, groups, sqdtype, dbm.ptr() + groups * CV_ELEM_SIZE(dtype)));

        if (haveMask)
            nz = saturate_cast<int>(funcs[0](Mat(1, groups, CV_32SC1, dbm.ptr() +
                                                 groups * (CV_ELEM_SIZE(dtype) +
                                                           CV_ELEM_SIZE(sqdtype))))[0]);
    }

    double total = nz != 0 ? 1.0 / nz : 0;
    int k, j;
    for (int i = 0; i < cn; ++i)
    {
        mean[i] *= total;
        stddev[i] = std::sqrt(std::max(stddev[i] * total - mean[i] * mean[i] , 0.));
    }

    for( j = 0; j < 2; j++ )
    {
        const double * const sptr = j == 0 ? &mean[0] : &stddev[0];
        _OutputArray _dst = j == 0 ? _mean : _sdv;
        if( !_dst.needed() )
            continue;

        if( !_dst.fixedSize() )
            _dst.create(cn, 1, CV_64F, -1, true);
        Mat dst = _dst.getMat();
        int dcn = (int)dst.total();
        CV_Assert( dst.type() == CV_64F && dst.isContinuous() &&
                   (dst.cols == 1 || dst.rows == 1) && dcn >= cn );
        double* dptr = dst.ptr<double>();
        for( k = 0; k < cn; k++ )
            dptr[k] = sptr[k];
        for( ; k < dcn; k++ )
            dptr[k] = 0;
    }

    return true;
}
#endif

#ifdef HAVE_OPENVX
    static bool openvx_meanStdDev(Mat& src, OutputArray _mean, OutputArray _sdv, Mat& mask)
    {
        size_t total_size = src.total();
        int rows = src.size[0], cols = rows ? (int)(total_size / rows) : 0;
        if (src.type() != CV_8UC1|| !mask.empty() ||
               (src.dims != 2 && !(src.isContinuous() && cols > 0 && (size_t)rows*cols == total_size))
           )
        return false;

        try
        {
            ivx::Context ctx = ovx::getOpenVXContext();
#ifndef VX_VERSION_1_1
            if (ctx.vendorID() == VX_ID_KHRONOS)
                return false; // Do not use OpenVX meanStdDev estimation for sample 1.0.1 implementation due to lack of accuracy
#endif

            ivx::Image
                ia = ivx::Image::createFromHandle(ctx, VX_DF_IMAGE_U8,
                    ivx::Image::createAddressing(cols, rows, 1, (vx_int32)(src.step[0])), src.ptr());

            vx_float32 mean_temp, stddev_temp;
            ivx::IVX_CHECK_STATUS(vxuMeanStdDev(ctx, ia, &mean_temp, &stddev_temp));

            if (_mean.needed())
            {
                if (!_mean.fixedSize())
                    _mean.create(1, 1, CV_64F, -1, true);
                Mat mean = _mean.getMat();
                CV_Assert(mean.type() == CV_64F && mean.isContinuous() &&
                    (mean.cols == 1 || mean.rows == 1) && mean.total() >= 1);
                double *pmean = mean.ptr<double>();
                pmean[0] = mean_temp;
                for (int c = 1; c < (int)mean.total(); c++)
                    pmean[c] = 0;
            }

            if (_sdv.needed())
            {
                if (!_sdv.fixedSize())
                    _sdv.create(1, 1, CV_64F, -1, true);
                Mat stddev = _sdv.getMat();
                CV_Assert(stddev.type() == CV_64F && stddev.isContinuous() &&
                    (stddev.cols == 1 || stddev.rows == 1) && stddev.total() >= 1);
                double *pstddev = stddev.ptr<double>();
                pstddev[0] = stddev_temp;
                for (int c = 1; c < (int)stddev.total(); c++)
                    pstddev[c] = 0;
            }
        }
        catch (ivx::RuntimeError & e)
        {
            VX_DbgThrow(e.what());
        }
        catch (ivx::WrapperError & e)
        {
            VX_DbgThrow(e.what());
        }

        return true;
    }
#endif

#ifdef HAVE_IPP
static bool ipp_meanStdDev(Mat& src, OutputArray _mean, OutputArray _sdv, Mat& mask)
{
    CV_INSTRUMENT_REGION_IPP()

#if IPP_VERSION_X100 >= 700
    int cn = src.channels();

#if IPP_VERSION_X100 < 201801
    // IPP_DISABLE: C3C functions can read outside of allocated memory
    if (cn > 1)
        return false;
#endif

    size_t total_size = src.total();
    int rows = src.size[0], cols = rows ? (int)(total_size/rows) : 0;
    if( src.dims == 2 || (src.isContinuous() && mask.isContinuous() && cols > 0 && (size_t)rows*cols == total_size) )
    {
        Ipp64f mean_temp[3];
        Ipp64f stddev_temp[3];
        Ipp64f *pmean = &mean_temp[0];
        Ipp64f *pstddev = &stddev_temp[0];
        Mat mean, stddev;
        int dcn_mean = -1;
        if( _mean.needed() )
        {
            if( !_mean.fixedSize() )
                _mean.create(cn, 1, CV_64F, -1, true);
            mean = _mean.getMat();
            dcn_mean = (int)mean.total();
            pmean = mean.ptr<Ipp64f>();
        }
        int dcn_stddev = -1;
        if( _sdv.needed() )
        {
            if( !_sdv.fixedSize() )
                _sdv.create(cn, 1, CV_64F, -1, true);
            stddev = _sdv.getMat();
            dcn_stddev = (int)stddev.total();
            pstddev = stddev.ptr<Ipp64f>();
        }
        for( int c = cn; c < dcn_mean; c++ )
            pmean[c] = 0;
        for( int c = cn; c < dcn_stddev; c++ )
            pstddev[c] = 0;
        IppiSize sz = { cols, rows };
        int type = src.type();
        if( !mask.empty() )
        {
            typedef IppStatus (CV_STDCALL* ippiMaskMeanStdDevFuncC1)(const void *, int, const void *, int, IppiSize, Ipp64f *, Ipp64f *);
            ippiMaskMeanStdDevFuncC1 ippiMean_StdDev_C1MR =
            type == CV_8UC1 ? (ippiMaskMeanStdDevFuncC1)ippiMean_StdDev_8u_C1MR :
            type == CV_16UC1 ? (ippiMaskMeanStdDevFuncC1)ippiMean_StdDev_16u_C1MR :
            type == CV_32FC1 ? (ippiMaskMeanStdDevFuncC1)ippiMean_StdDev_32f_C1MR :
            0;
            if( ippiMean_StdDev_C1MR )
            {
                if( CV_INSTRUMENT_FUN_IPP(ippiMean_StdDev_C1MR, src.ptr(), (int)src.step[0], mask.ptr(), (int)mask.step[0], sz, pmean, pstddev) >= 0 )
                {
                    return true;
                }
            }
            typedef IppStatus (CV_STDCALL* ippiMaskMeanStdDevFuncC3)(const void *, int, const void *, int, IppiSize, int, Ipp64f *, Ipp64f *);
            ippiMaskMeanStdDevFuncC3 ippiMean_StdDev_C3CMR =
            type == CV_8UC3 ? (ippiMaskMeanStdDevFuncC3)ippiMean_StdDev_8u_C3CMR :
            type == CV_16UC3 ? (ippiMaskMeanStdDevFuncC3)ippiMean_StdDev_16u_C3CMR :
            type == CV_32FC3 ? (ippiMaskMeanStdDevFuncC3)ippiMean_StdDev_32f_C3CMR :
            0;
            if( ippiMean_StdDev_C3CMR )
            {
                if( CV_INSTRUMENT_FUN_IPP(ippiMean_StdDev_C3CMR, src.ptr(), (int)src.step[0], mask.ptr(), (int)mask.step[0], sz, 1, &pmean[0], &pstddev[0]) >= 0 &&
                    CV_INSTRUMENT_FUN_IPP(ippiMean_StdDev_C3CMR, src.ptr(), (int)src.step[0], mask.ptr(), (int)mask.step[0], sz, 2, &pmean[1], &pstddev[1]) >= 0 &&
                    CV_INSTRUMENT_FUN_IPP(ippiMean_StdDev_C3CMR, src.ptr(), (int)src.step[0], mask.ptr(), (int)mask.step[0], sz, 3, &pmean[2], &pstddev[2]) >= 0 )
                {
                    return true;
                }
            }
        }
        else
        {
            typedef IppStatus (CV_STDCALL* ippiMeanStdDevFuncC1)(const void *, int, IppiSize, Ipp64f *, Ipp64f *);
            ippiMeanStdDevFuncC1 ippiMean_StdDev_C1R =
            type == CV_8UC1 ? (ippiMeanStdDevFuncC1)ippiMean_StdDev_8u_C1R :
            type == CV_16UC1 ? (ippiMeanStdDevFuncC1)ippiMean_StdDev_16u_C1R :
#if (IPP_VERSION_X100 >= 810)
            type == CV_32FC1 ? (ippiMeanStdDevFuncC1)ippiMean_StdDev_32f_C1R ://Aug 2013: bug in IPP 7.1, 8.0
#endif
            0;
            if( ippiMean_StdDev_C1R )
            {
                if( CV_INSTRUMENT_FUN_IPP(ippiMean_StdDev_C1R, src.ptr(), (int)src.step[0], sz, pmean, pstddev) >= 0 )
                {
                    return true;
                }
            }
            typedef IppStatus (CV_STDCALL* ippiMeanStdDevFuncC3)(const void *, int, IppiSize, int, Ipp64f *, Ipp64f *);
            ippiMeanStdDevFuncC3 ippiMean_StdDev_C3CR =
            type == CV_8UC3 ? (ippiMeanStdDevFuncC3)ippiMean_StdDev_8u_C3CR :
            type == CV_16UC3 ? (ippiMeanStdDevFuncC3)ippiMean_StdDev_16u_C3CR :
            type == CV_32FC3 ? (ippiMeanStdDevFuncC3)ippiMean_StdDev_32f_C3CR :
            0;
            if( ippiMean_StdDev_C3CR )
            {
                if( CV_INSTRUMENT_FUN_IPP(ippiMean_StdDev_C3CR, src.ptr(), (int)src.step[0], sz, 1, &pmean[0], &pstddev[0]) >= 0 &&
                    CV_INSTRUMENT_FUN_IPP(ippiMean_StdDev_C3CR, src.ptr(), (int)src.step[0], sz, 2, &pmean[1], &pstddev[1]) >= 0 &&
                    CV_INSTRUMENT_FUN_IPP(ippiMean_StdDev_C3CR, src.ptr(), (int)src.step[0], sz, 3, &pmean[2], &pstddev[2]) >= 0 )
                {
                    return true;
                }
            }
        }
    }
#else
    CV_UNUSED(src); CV_UNUSED(_mean); CV_UNUSED(_sdv); CV_UNUSED(mask);
#endif
    return false;
}
#endif

} // cv::

void cv::meanStdDev( InputArray _src, OutputArray _mean, OutputArray _sdv, InputArray _mask )
{
    CV_INSTRUMENT_REGION()

    CV_Assert(!_src.empty());
    CV_Assert( _mask.empty() || _mask.type() == CV_8UC1 );

    CV_OCL_RUN(OCL_PERFORMANCE_CHECK(_src.isUMat()) && _src.dims() <= 2,
               ocl_meanStdDev(_src, _mean, _sdv, _mask))

    Mat src = _src.getMat(), mask = _mask.getMat();

    CV_OVX_RUN(!ovx::skipSmallImages<VX_KERNEL_MEAN_STDDEV>(src.cols, src.rows),
               openvx_meanStdDev(src, _mean, _sdv, mask))

    CV_IPP_RUN(IPP_VERSION_X100 >= 700, ipp_meanStdDev(src, _mean, _sdv, mask));

    int k, cn = src.channels(), depth = src.depth();

    SumSqrFunc func = getSumSqrTab(depth);

    CV_Assert( func != 0 );

    const Mat* arrays[] = {&src, &mask, 0};
    uchar* ptrs[2];
    NAryMatIterator it(arrays, ptrs);
    int total = (int)it.size, blockSize = total, intSumBlockSize = 0;
    int j, count = 0, nz0 = 0;
    AutoBuffer<double> _buf(cn*4);
    double *s = (double*)_buf.data(), *sq = s + cn;
    int *sbuf = (int*)s, *sqbuf = (int*)sq;
    bool blockSum = depth <= CV_16S, blockSqSum = depth <= CV_8S;
    size_t esz = 0;

    for( k = 0; k < cn; k++ )
        s[k] = sq[k] = 0;

    if( blockSum )
    {
        intSumBlockSize = 1 << 15;
        blockSize = std::min(blockSize, intSumBlockSize);
        sbuf = (int*)(sq + cn);
        if( blockSqSum )
            sqbuf = sbuf + cn;
        for( k = 0; k < cn; k++ )
            sbuf[k] = sqbuf[k] = 0;
        esz = src.elemSize();
    }

    for( size_t i = 0; i < it.nplanes; i++, ++it )
    {
        for( j = 0; j < total; j += blockSize )
        {
            int bsz = std::min(total - j, blockSize);
            int nz = func( ptrs[0], ptrs[1], (uchar*)sbuf, (uchar*)sqbuf, bsz, cn );
            count += nz;
            nz0 += nz;
            if( blockSum && (count + blockSize >= intSumBlockSize || (i+1 >= it.nplanes && j+bsz >= total)) )
            {
                for( k = 0; k < cn; k++ )
                {
                    s[k] += sbuf[k];
                    sbuf[k] = 0;
                }
                if( blockSqSum )
                {
                    for( k = 0; k < cn; k++ )
                    {
                        sq[k] += sqbuf[k];
                        sqbuf[k] = 0;
                    }
                }
                count = 0;
            }
            ptrs[0] += bsz*esz;
            if( ptrs[1] )
                ptrs[1] += bsz;
        }
    }

    double scale = nz0 ? 1./nz0 : 0.;
    for( k = 0; k < cn; k++ )
    {
        s[k] *= scale;
        sq[k] = std::sqrt(std::max(sq[k]*scale - s[k]*s[k], 0.));
    }

    for( j = 0; j < 2; j++ )
    {
        const double* sptr = j == 0 ? s : sq;
        _OutputArray _dst = j == 0 ? _mean : _sdv;
        if( !_dst.needed() )
            continue;

        if( !_dst.fixedSize() )
            _dst.create(cn, 1, CV_64F, -1, true);
        Mat dst = _dst.getMat();
        int dcn = (int)dst.total();
        CV_Assert( dst.type() == CV_64F && dst.isContinuous() &&
                   (dst.cols == 1 || dst.rows == 1) && dcn >= cn );
        double* dptr = dst.ptr<double>();
        for( k = 0; k < cn; k++ )
            dptr[k] = sptr[k];
        for( ; k < dcn; k++ )
            dptr[k] = 0;
    }
}
