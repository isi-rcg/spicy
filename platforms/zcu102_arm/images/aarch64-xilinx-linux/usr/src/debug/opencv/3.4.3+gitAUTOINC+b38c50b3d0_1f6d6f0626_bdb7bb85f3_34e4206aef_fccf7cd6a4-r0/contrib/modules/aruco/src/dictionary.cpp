/*
By downloading, copying, installing or using the software you agree to this
license. If you do not agree to this license, do not download, install,
copy or use the software.

                          License Agreement
               For Open Source Computer Vision Library
                       (3-clause BSD License)

Copyright (C) 2013, OpenCV Foundation, all rights reserved.
Third party copyrights are property of their respective owners.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  * Neither the names of the copyright holders nor the names of the contributors
    may be used to endorse or promote products derived from this software
    without specific prior written permission.

This software is provided by the copyright holders and contributors "as is" and
any express or implied warranties, including, but not limited to, the implied
warranties of merchantability and fitness for a particular purpose are
disclaimed. In no event shall copyright holders or contributors be liable for
any direct, indirect, incidental, special, exemplary, or consequential damages
(including, but not limited to, procurement of substitute goods or services;
loss of use, data, or profits; or business interruption) however caused
and on any theory of liability, whether in contract, strict liability,
or tort (including negligence or otherwise) arising in any way out of
the use of this software, even if advised of the possibility of such damage.
*/

#include "precomp.hpp"
#include "opencv2/aruco/dictionary.hpp"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "predefined_dictionaries.hpp"
#include "predefined_dictionaries_apriltag.hpp"
#include "opencv2/core/hal/hal.hpp"

namespace cv {
namespace aruco {

using namespace std;


/**
  */
Dictionary::Dictionary(const Ptr<Dictionary> &_dictionary) {
    markerSize = _dictionary->markerSize;
    maxCorrectionBits = _dictionary->maxCorrectionBits;
    bytesList = _dictionary->bytesList.clone();
}


/**
  */
Dictionary::Dictionary(const Mat &_bytesList, int _markerSize, int _maxcorr) {
    markerSize = _markerSize;
    maxCorrectionBits = _maxcorr;
    bytesList = _bytesList;
}


/**
 */
Ptr<Dictionary> Dictionary::create(int nMarkers, int markerSize, int randomSeed) {
    const Ptr<Dictionary> baseDictionary = makePtr<Dictionary>();
    return create(nMarkers, markerSize, baseDictionary, randomSeed);
}


/**
 */
Ptr<Dictionary> Dictionary::create(int nMarkers, int markerSize,
                                   const Ptr<Dictionary> &baseDictionary, int randomSeed) {

    return generateCustomDictionary(nMarkers, markerSize, baseDictionary, randomSeed);
}


/**
 */
Ptr<Dictionary> Dictionary::get(int dict) {
    return getPredefinedDictionary(dict);
}


/**
 */
bool Dictionary::identify(const Mat &onlyBits, int &idx, int &rotation,
                          double maxCorrectionRate) const {

    CV_Assert(onlyBits.rows == markerSize && onlyBits.cols == markerSize);

    int maxCorrectionRecalculed = int(double(maxCorrectionBits) * maxCorrectionRate);

    // get as a byte list
    Mat candidateBytes = getByteListFromBits(onlyBits);

    idx = -1; // by default, not found

    // search closest marker in dict
    for(int m = 0; m < bytesList.rows; m++) {
        int currentMinDistance = markerSize * markerSize + 1;
        int currentRotation = -1;
        for(unsigned int r = 0; r < 4; r++) {
            int currentHamming = cv::hal::normHamming(
                    bytesList.ptr(m)+r*candidateBytes.cols,
                    candidateBytes.ptr(),
                    candidateBytes.cols);

            if(currentHamming < currentMinDistance) {
                currentMinDistance = currentHamming;
                currentRotation = r;
            }
        }

        // if maxCorrection is fullfilled, return this one
        if(currentMinDistance <= maxCorrectionRecalculed) {
            idx = m;
            rotation = currentRotation;
            break;
        }
    }

    return idx != -1;
}


/**
  */
int Dictionary::getDistanceToId(InputArray bits, int id, bool allRotations) const {

    CV_Assert(id >= 0 && id < bytesList.rows);

    unsigned int nRotations = 4;
    if(!allRotations) nRotations = 1;

    Mat candidateBytes = getByteListFromBits(bits.getMat());
    int currentMinDistance = int(bits.total() * bits.total());
    for(unsigned int r = 0; r < nRotations; r++) {
        int currentHamming = cv::hal::normHamming(
                bytesList.ptr(id) + r*candidateBytes.cols,
                candidateBytes.ptr(),
                candidateBytes.cols);

        if(currentHamming < currentMinDistance) {
            currentMinDistance = currentHamming;
        }
    }
    return currentMinDistance;
}



/**
 * @brief Draw a canonical marker image
 */
void Dictionary::drawMarker(int id, int sidePixels, OutputArray _img, int borderBits) const {

    CV_Assert(sidePixels >= (markerSize + 2*borderBits));
    CV_Assert(id < bytesList.rows);
    CV_Assert(borderBits > 0);

    _img.create(sidePixels, sidePixels, CV_8UC1);

    // create small marker with 1 pixel per bin
    Mat tinyMarker(markerSize + 2 * borderBits, markerSize + 2 * borderBits, CV_8UC1,
                   Scalar::all(0));
    Mat innerRegion = tinyMarker.rowRange(borderBits, tinyMarker.rows - borderBits)
                          .colRange(borderBits, tinyMarker.cols - borderBits);
    // put inner bits
    Mat bits = 255 * getBitsFromByteList(bytesList.rowRange(id, id + 1), markerSize);
    CV_Assert(innerRegion.total() == bits.total());
    bits.copyTo(innerRegion);

    // resize tiny marker to output size
    cv::resize(tinyMarker, _img.getMat(), _img.getMat().size(), 0, 0, INTER_NEAREST);
}




/**
  * @brief Transform matrix of bits to list of bytes in the 4 rotations
  */
Mat Dictionary::getByteListFromBits(const Mat &bits) {
    // integer ceil
    int nbytes = (bits.cols * bits.rows + 8 - 1) / 8;

    Mat candidateByteList(1, nbytes, CV_8UC4, Scalar::all(0));
    unsigned char currentBit = 0;
    int currentByte = 0;

    // the 4 rotations
    uchar* rot0 = candidateByteList.ptr();
    uchar* rot1 = candidateByteList.ptr() + 1*nbytes;
    uchar* rot2 = candidateByteList.ptr() + 2*nbytes;
    uchar* rot3 = candidateByteList.ptr() + 3*nbytes;

    for(int row = 0; row < bits.rows; row++) {
        for(int col = 0; col < bits.cols; col++) {
            // circular shift
            rot0[currentByte] <<= 1;
            rot1[currentByte] <<= 1;
            rot2[currentByte] <<= 1;
            rot3[currentByte] <<= 1;
            // set bit
            rot0[currentByte] |= bits.at<uchar>(row, col);
            rot1[currentByte] |= bits.at<uchar>(col, bits.cols - 1 - row);
            rot2[currentByte] |= bits.at<uchar>(bits.rows - 1 - row, bits.cols - 1 - col);
            rot3[currentByte] |= bits.at<uchar>(bits.rows - 1 - col, row);
            currentBit++;
            if(currentBit == 8) {
                // next byte
                currentBit = 0;
                currentByte++;
            }
        }
    }
    return candidateByteList;
}



/**
  * @brief Transform list of bytes to matrix of bits
  */
Mat Dictionary::getBitsFromByteList(const Mat &byteList, int markerSize) {
    CV_Assert(byteList.total() > 0 &&
              byteList.total() >= (unsigned int)markerSize * markerSize / 8 &&
              byteList.total() <= (unsigned int)markerSize * markerSize / 8 + 1);
    Mat bits(markerSize, markerSize, CV_8UC1, Scalar::all(0));

    unsigned char base2List[] = { 128, 64, 32, 16, 8, 4, 2, 1 };
    int currentByteIdx = 0;
    // we only need the bytes in normal rotation
    unsigned char currentByte = byteList.ptr()[0];
    int currentBit = 0;
    for(int row = 0; row < bits.rows; row++) {
        for(int col = 0; col < bits.cols; col++) {
            if(currentByte >= base2List[currentBit]) {
                bits.at< unsigned char >(row, col) = 1;
                currentByte -= base2List[currentBit];
            }
            currentBit++;
            if(currentBit == 8) {
                currentByteIdx++;
                currentByte = byteList.ptr()[currentByteIdx];
                // if not enough bits for one more byte, we are in the end
                // update bit position accordingly
                if(8 * (currentByteIdx + 1) > (int)bits.total())
                    currentBit = 8 * (currentByteIdx + 1) - (int)bits.total();
                else
                    currentBit = 0; // ok, bits enough for next byte
            }
        }
    }
    return bits;
}




// DictionaryData constructors calls
const Dictionary DICT_ARUCO_DATA = Dictionary(Mat(1024, (5*5 + 7)/8, CV_8UC4, (uchar*)DICT_ARUCO_BYTES), 5, 0);

const Dictionary DICT_4X4_50_DATA = Dictionary(Mat(50, (4*4 + 7)/8, CV_8UC4, (uchar*)DICT_4X4_1000_BYTES), 4, 1);
const Dictionary DICT_4X4_100_DATA = Dictionary(Mat(100, (4*4 + 7)/8, CV_8UC4, (uchar*)DICT_4X4_1000_BYTES), 4, 1);
const Dictionary DICT_4X4_250_DATA = Dictionary(Mat(250, (4*4 + 7)/8, CV_8UC4, (uchar*)DICT_4X4_1000_BYTES), 4, 1);
const Dictionary DICT_4X4_1000_DATA = Dictionary(Mat(1000, (4*4 + 7)/8, CV_8UC4, (uchar*)DICT_4X4_1000_BYTES), 4, 0);

const Dictionary DICT_5X5_50_DATA = Dictionary(Mat(50, (5*5 + 7)/8, CV_8UC4, (uchar*)DICT_5X5_1000_BYTES), 5, 3);
const Dictionary DICT_5X5_100_DATA = Dictionary(Mat(100, (5*5 + 7)/8, CV_8UC4, (uchar*)DICT_5X5_1000_BYTES), 5, 3);
const Dictionary DICT_5X5_250_DATA = Dictionary(Mat(250, (5*5 + 7)/8, CV_8UC4, (uchar*)DICT_5X5_1000_BYTES), 5, 2);
const Dictionary DICT_5X5_1000_DATA = Dictionary(Mat(1000, (5*5 + 7)/8, CV_8UC4, (uchar*)DICT_5X5_1000_BYTES), 5, 2);

const Dictionary DICT_6X6_50_DATA = Dictionary(Mat(50, (6*6 + 7)/8 ,CV_8UC4, (uchar*)DICT_6X6_1000_BYTES), 6, 6);
const Dictionary DICT_6X6_100_DATA = Dictionary(Mat(100, (6*6 + 7)/8 ,CV_8UC4, (uchar*)DICT_6X6_1000_BYTES), 6, 5);
const Dictionary DICT_6X6_250_DATA = Dictionary(Mat(250, (6*6 + 7)/8 ,CV_8UC4, (uchar*)DICT_6X6_1000_BYTES), 6, 5);
const Dictionary DICT_6X6_1000_DATA = Dictionary(Mat(1000, (6*6 + 7)/8 ,CV_8UC4, (uchar*)DICT_6X6_1000_BYTES), 6, 4);

const Dictionary DICT_7X7_50_DATA = Dictionary(Mat(50, (7*7 + 7)/8 ,CV_8UC4, (uchar*)DICT_7X7_1000_BYTES), 7, 9);
const Dictionary DICT_7X7_100_DATA = Dictionary(Mat(100, (7*7 + 7)/8 ,CV_8UC4, (uchar*)DICT_7X7_1000_BYTES), 7, 8);
const Dictionary DICT_7X7_250_DATA = Dictionary(Mat(250, (7*7 + 7)/8 ,CV_8UC4, (uchar*)DICT_7X7_1000_BYTES), 7, 8);
const Dictionary DICT_7X7_1000_DATA = Dictionary(Mat(1000, (7*7 + 7)/8 ,CV_8UC4, (uchar*)DICT_7X7_1000_BYTES), 7, 6);

const Dictionary DICT_APRILTAG_16h5_DATA = Dictionary(Mat(30, (4*4 + 7)/8, CV_8UC4, (uchar*)DICT_APRILTAG_16h5_BYTES), 4, 0);
const Dictionary DICT_APRILTAG_25h9_DATA = Dictionary(Mat(35, (5*5 + 7)/8, CV_8UC4, (uchar*)DICT_APRILTAG_25h9_BYTES), 5, 0);
const Dictionary DICT_APRILTAG_36h10_DATA = Dictionary(Mat(2320, (6*6 + 7)/8, CV_8UC4, (uchar*)DICT_APRILTAG_36h10_BYTES), 6, 0);
const Dictionary DICT_APRILTAG_36h11_DATA = Dictionary(Mat(587, (6*6 + 7)/8, CV_8UC4, (uchar*)DICT_APRILTAG_36h11_BYTES), 6, 0);


Ptr<Dictionary> getPredefinedDictionary(PREDEFINED_DICTIONARY_NAME name) {
    switch(name) {

    case DICT_ARUCO_ORIGINAL:
        return makePtr<Dictionary>(DICT_ARUCO_DATA);

    case DICT_4X4_50:
        return makePtr<Dictionary>(DICT_4X4_50_DATA);
    case DICT_4X4_100:
        return makePtr<Dictionary>(DICT_4X4_100_DATA);
    case DICT_4X4_250:
        return makePtr<Dictionary>(DICT_4X4_250_DATA);
    case DICT_4X4_1000:
        return makePtr<Dictionary>(DICT_4X4_1000_DATA);

    case DICT_5X5_50:
        return makePtr<Dictionary>(DICT_5X5_50_DATA);
    case DICT_5X5_100:
        return makePtr<Dictionary>(DICT_5X5_100_DATA);
    case DICT_5X5_250:
        return makePtr<Dictionary>(DICT_5X5_250_DATA);
    case DICT_5X5_1000:
        return makePtr<Dictionary>(DICT_5X5_1000_DATA);

    case DICT_6X6_50:
        return makePtr<Dictionary>(DICT_6X6_50_DATA);
    case DICT_6X6_100:
        return makePtr<Dictionary>(DICT_6X6_100_DATA);
    case DICT_6X6_250:
        return makePtr<Dictionary>(DICT_6X6_250_DATA);
    case DICT_6X6_1000:
        return makePtr<Dictionary>(DICT_6X6_1000_DATA);

    case DICT_7X7_50:
        return makePtr<Dictionary>(DICT_7X7_50_DATA);
    case DICT_7X7_100:
        return makePtr<Dictionary>(DICT_7X7_100_DATA);
    case DICT_7X7_250:
        return makePtr<Dictionary>(DICT_7X7_250_DATA);
    case DICT_7X7_1000:
        return makePtr<Dictionary>(DICT_7X7_1000_DATA);

    case DICT_APRILTAG_16h5:
        return makePtr<Dictionary>(DICT_APRILTAG_16h5_DATA);
    case DICT_APRILTAG_25h9:
        return makePtr<Dictionary>(DICT_APRILTAG_25h9_DATA);
    case DICT_APRILTAG_36h10:
        return makePtr<Dictionary>(DICT_APRILTAG_36h10_DATA);
    case DICT_APRILTAG_36h11:
        return makePtr<Dictionary>(DICT_APRILTAG_36h11_DATA);

    }
    return makePtr<Dictionary>(DICT_4X4_50_DATA);
}


Ptr<Dictionary> getPredefinedDictionary(int dict) {
    return getPredefinedDictionary(PREDEFINED_DICTIONARY_NAME(dict));
}


/**
 * @brief Generates a random marker Mat of size markerSize x markerSize
 */
static Mat _generateRandomMarker(int markerSize, RNG &rng) {
    Mat marker(markerSize, markerSize, CV_8UC1, Scalar::all(0));
    for(int i = 0; i < markerSize; i++) {
        for(int j = 0; j < markerSize; j++) {
            unsigned char bit = (unsigned char) (rng.uniform(0,2));
            marker.at< unsigned char >(i, j) = bit;
        }
    }
    return marker;
}

/**
 * @brief Calculate selfDistance of the codification of a marker Mat. Self distance is the Hamming
 * distance of the marker to itself in the other rotations.
 * See S. Garrido-Jurado, R. Muñoz-Salinas, F. J. Madrid-Cuevas, and M. J. Marín-Jiménez. 2014.
 * "Automatic generation and detection of highly reliable fiducial markers under occlusion".
 * Pattern Recogn. 47, 6 (June 2014), 2280-2292. DOI=10.1016/j.patcog.2014.01.005
 */
static int _getSelfDistance(const Mat &marker) {
    Mat bytes = Dictionary::getByteListFromBits(marker);
    int minHamming = (int)marker.total() + 1;
    for(int r = 1; r < 4; r++) {
        int currentHamming = cv::hal::normHamming(bytes.ptr(), bytes.ptr() + bytes.cols*r, bytes.cols);
        if(currentHamming < minHamming) minHamming = currentHamming;
    }
    return minHamming;
}

/**
 */
Ptr<Dictionary> generateCustomDictionary(int nMarkers, int markerSize,
                                         const Ptr<Dictionary> &baseDictionary, int randomSeed) {
    RNG rng((uint64)(randomSeed));

    Ptr<Dictionary> out = makePtr<Dictionary>();
    out->markerSize = markerSize;

    // theoretical maximum intermarker distance
    // See S. Garrido-Jurado, R. Muñoz-Salinas, F. J. Madrid-Cuevas, and M. J. Marín-Jiménez. 2014.
    // "Automatic generation and detection of highly reliable fiducial markers under occlusion".
    // Pattern Recogn. 47, 6 (June 2014), 2280-2292. DOI=10.1016/j.patcog.2014.01.005
    int C = (int)std::floor(float(markerSize * markerSize) / 4.f);
    int tau = 2 * (int)std::floor(float(C) * 4.f / 3.f);

    // if baseDictionary is provided, calculate its intermarker distance
    if(baseDictionary->bytesList.rows > 0) {
        CV_Assert(baseDictionary->markerSize == markerSize);
        out->bytesList = baseDictionary->bytesList.clone();

        int minDistance = markerSize * markerSize + 1;
        for(int i = 0; i < out->bytesList.rows; i++) {
            Mat markerBytes = out->bytesList.rowRange(i, i + 1);
            Mat markerBits = Dictionary::getBitsFromByteList(markerBytes, markerSize);
            minDistance = min(minDistance, _getSelfDistance(markerBits));
            for(int j = i + 1; j < out->bytesList.rows; j++) {
                minDistance = min(minDistance, out->getDistanceToId(markerBits, j));
            }
        }
        tau = minDistance;
    }

    // current best option
    int bestTau = 0;
    Mat bestMarker;

    // after these number of unproductive iterations, the best option is accepted
    const int maxUnproductiveIterations = 5000;
    int unproductiveIterations = 0;

    while(out->bytesList.rows < nMarkers) {
        Mat currentMarker = _generateRandomMarker(markerSize, rng);

        int selfDistance = _getSelfDistance(currentMarker);
        int minDistance = selfDistance;

        // if self distance is better or equal than current best option, calculate distance
        // to previous accepted markers
        if(selfDistance >= bestTau) {
            for(int i = 0; i < out->bytesList.rows; i++) {
                int currentDistance = out->getDistanceToId(currentMarker, i);
                minDistance = min(currentDistance, minDistance);
                if(minDistance <= bestTau) {
                    break;
                }
            }
        }

        // if distance is high enough, accept the marker
        if(minDistance >= tau) {
            unproductiveIterations = 0;
            bestTau = 0;
            Mat bytes = Dictionary::getByteListFromBits(currentMarker);
            out->bytesList.push_back(bytes);
        } else {
            unproductiveIterations++;

            // if distance is not enough, but is better than the current best option
            if(minDistance > bestTau) {
                bestTau = minDistance;
                bestMarker = currentMarker;
            }

            // if number of unproductive iterarions has been reached, accept the current best option
            if(unproductiveIterations == maxUnproductiveIterations) {
                unproductiveIterations = 0;
                tau = bestTau;
                bestTau = 0;
                Mat bytes = Dictionary::getByteListFromBits(bestMarker);
                out->bytesList.push_back(bytes);
            }
        }
    }

    // update the maximum number of correction bits for the generated dictionary
    out->maxCorrectionBits = (tau - 1) / 2;

    return out;
}


/**
 */
Ptr<Dictionary> generateCustomDictionary(int nMarkers, int markerSize, int randomSeed) {
    Ptr<Dictionary> baseDictionary = makePtr<Dictionary>();
    return generateCustomDictionary(nMarkers, markerSize, baseDictionary, randomSeed);
}


}
}
