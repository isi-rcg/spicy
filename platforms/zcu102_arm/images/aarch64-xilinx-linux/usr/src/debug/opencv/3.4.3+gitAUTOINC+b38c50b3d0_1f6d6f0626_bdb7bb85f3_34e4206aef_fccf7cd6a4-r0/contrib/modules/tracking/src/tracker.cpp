/*M///////////////////////////////////////////////////////////////////////////////////////
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

#include "precomp.hpp"

namespace cv
{

/*
 *  Tracker
 */

Tracker::~Tracker()
{
}

bool Tracker::init( InputArray image, const Rect2d& boundingBox )
{

  if( isInit )
  {
    return false;
  }

  if( image.empty() )
    return false;

  sampler = Ptr<TrackerSampler>( new TrackerSampler() );
  featureSet = Ptr<TrackerFeatureSet>( new TrackerFeatureSet() );
  model = Ptr<TrackerModel>();

  bool initTracker = initImpl( image.getMat(), boundingBox );

  //check if the model component is initialized
  if( model == 0 )
  {
    CV_Error( -1, "The model is not initialized" );
  }

  if( initTracker )
  {
    isInit = true;
  }

  return initTracker;
}

bool Tracker::update( InputArray image, Rect2d& boundingBox )
{

  if( !isInit )
  {
    return false;
  }

  if( image.empty() )
    return false;

  return updateImpl( image.getMat(), boundingBox );
}

} /* namespace cv */
