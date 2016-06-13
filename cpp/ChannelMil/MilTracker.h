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
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009-2011, Willow Garage Inc., all rights reserved.
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


#pragma once

#include <fstream>
#include <iostream>
#include <limits>
#include <math.h>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <memory>
#include <utils/SysTimer.h>
#include "opencv2/highgui/highgui_c.h"

#include "sample.h"
#include "milutil.h"
#include "ClfMilBoost.h"
#include "features/Icf/IntegralFeatures.h"
#include "features/Icf/IntegralFeaturesObserver.h"
#include "utils/XmlConf.h"

#include "externals/FastSegmentation/segment.h"

#include "tracking/TrackingLogger.h"
#include "tracking/trackingMacros.h"
class VCA_API MilTrackerParams
{
public:
  MilTrackerParams();

  MilTrackerParams(Vca::XmlConf& config);

  float _posRadius;
  float _posRadiusScaleBigger;
  float _posRadiusScaleSmaller;
  float _negRadius;
  float _negRadiusScaleBigger;
  float _negRadiusScaleSmaller;
  float _searchRadius;
  float _searchRadiusScaleBigger;
  float _searchRadiusScaleSmaller;
  float _scaleStep;
  float _minscale;
  float _maxscale;
  float _posMinOverlap;
  float _negMaxOverlap;
  bool _fixedSize;
  int _fixedHeight;
  int _fixedWidth;
  float _minhwRatio;
  float _maxhwRatio;


  //float _minScale;
  //float _maxScale;

  //   float _posradtrain; // radius for gathering positive instances
  //int _srchwinsz; // size of search window
  //float _init_postrainrad; // radius for gathering positive instances

  int _posmaxtrain; // max # of pos to train with
  int _searchmax;
  int _negnumtrain; // # negative samples to use during training, and init
  int _init_negnumtrain; // # negative samples to use during training, and init

  ClfMilBoostParams _clfparams;
  int _width;
  int _height;
  float _hwRatio;
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




class VCA_API MilTracker
{
public:

  MilTracker(shared_ptr<vca::icf::IntegralFeatures> feat);

  MilTracker(shared_ptr<vca::icf::IntegralFeatures> feat, MilTrackerParams& p, cv::Rect& bbox);


  ~MilTracker();

  cv::Rect getTrackBox();

  void setup(Vca::XmlConf& xmlconf);
  void setup(MilTrackerParams& p);

  bool init(cv::Rect& bbox);
  void logger(cv::Mat img, string folder, int frameNr);

  float track_frame();
  void track_GT_frame(cv::Rect curr_gt, float scale);
  void updateClf();// string folder, int frameNum);
  float classify(int x, int y, int w, int h);
  void updateClf(int x, int y, int w, int h, float trust = 1.f);

  void logSampleValues(std::vector<cv::Mat>& posFtrValues, std::vector<cv::Mat>& negFtrValues, string folder, int frameNum);
  void segmentDetBox(cv::Mat& img, cv::Rect trackBox);
  void selectSamples(vector<cv::Mat>&sampleFtrValues, vector< Sample >& samples);
  double kernelProfile_Epanechnikov(double x);

  //   void process( vector< cv::Mat >& integralMap ) override;

  void drawDebug(cv::Mat& img);

  void drawFeatures(cv::Mat& img);// , string folder, int frameNum);
  void drawNegSamples(cv::Mat& img);
  void drawPosSamples(cv::Mat& img);
  MilTrackerParams _trparams;

  int getCount();

protected:
  shared_ptr<vca::icf::IntegralFeatures>  _feat;

private:
  //  MilTracker(const MilTracker&);                 // Prevent copy-construction
  //   MilTracker& operator=(const MilTracker&);      // Prevent assignment

  void drawSamples(const std::vector<Sample>& samples, cv::Mat& img, cv::Scalar color);

  //void sampleImage( std::vector<Sample>& samples, std::vector<cv::Mat>& sampleFtrValues, int x, int y, float scale,
  //  int width, int height, int ncols, int nrows, float inrad, float inradscale, bool scaleIndipendent = false, float outrad = 0.f, float outradscale = 0.f, int maxnum = 1000000);
  void sampleImage(std::vector<Sample>& samples, std::vector<cv::Mat>& sampleFtrValues, int x, int y, float scale, int width, int height, int ncols, int nrows,
    float maxRadPixelRatio, float minScaleChange, float maxScaleChange, float minOverlap, float maxOverlap, int maxnum, float globalMinScale, float globalMaxScale);
  void generateSamples(std::vector<Sample>& samples, std::vector<cv::Mat>& sampleFtrValues, int x, int y, float scale, int width, int height, int ncols, int nrows,
    float maxRadPixelRatio, float minScaleChange, float maxScaleChange, float minOverlap, float maxOverlap, int maxnum, float globalMinScale, float globalMaxScale);
  // cv::Rect _curState;
  int _curX;
  int _curY;
  float _curScale;

  cv::Mat _segmentMask;


  HaarFtrParams _ftrParams;
  ClfMilBoost clsf;
  int _cnt;
  std::vector<Sample> _posx;
  std::vector<Sample> _negx;
  std::vector<Sample> _sample;
  std::vector<Sample> _clfsample;
  std::vector<cv::Mat> _posFtrValues;
  std::vector<cv::Mat> _negFtrValues;
  std::vector<cv::Mat> _sampleFtrValues;
  std::vector<cv::Mat> _clfsampleFtrValues;
  //    vector<float> prob;
};



/* End of file. */
