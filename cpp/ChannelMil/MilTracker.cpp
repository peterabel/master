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

#include <iomanip>
#include "MilTracker.h"
#include <numeric>




using namespace std;




//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
MilTrackerParams::MilTrackerParams()
{

  _posRadius = 0.2f;
  _posRadiusScaleBigger = 0.f;
  _posRadiusScaleSmaller = 0.025f;
  _negRadius = 3.f;
  _negRadiusScaleBigger = 0.5f;
  _negRadiusScaleSmaller = 0.1f;
  _searchRadius = 3.f;
  _searchRadiusScaleBigger = 0.05f;
  _searchRadiusScaleSmaller = 0.05f;
  _scaleStep = .025f;
  _posMinOverlap = .95f;
  _negMaxOverlap = .1f;
  _searchmax = 10000;
  _fixedSize = false;
  _fixedHeight = -1;
  _fixedWidth = -1;
  _minhwRatio = 1.5;

  //_posRadius = 5;
  //_posRadiusScale = .05f;
  //_negMinRadius = 15;
  //_negMinRadiusScale = .25f;
  //_negMaxRadius = 35;
  //_negMaxRadiusScale = .35f;
  //_searchRadius = 25;
  //_searchRadiusScale = .25f;

  ////_minScale = 0.5f;
  ////_maxScale = 2.f;
  //_scaleStep = 0.01f;

  _negnumtrain = 15;
  //			_posradtrain = 1;
  _posmaxtrain = 100000;
  _init_negnumtrain = 1000;
  //			_init_postrainrad = 3;

  //			_srchwinsz = 30;

  _clfparams._lRate = 0.7f;
  // _clfparams._lRate = 0.85f;
  //  _clfparams._numFeat = 250;
  _clfparams._numFeat.push_back(160);
  _clfparams._numSel = 160;

  _width = 64;
  _height = 192;

}

MilTrackerParams::MilTrackerParams(Vca::XmlConf& xmlconf)
{
  passert(xmlconf.setNode("mil"));

  passert(xmlconf.get("nNeg", _negnumtrain));
  passert(xmlconf.get("nPosMax", _posmaxtrain));
  passert(xmlconf.get("nSearchMax", _searchmax));
  passert(xmlconf.get("init_nNeg", _init_negnumtrain));

  passert(xmlconf.get("posRadius", _posRadius));
  float posRadiusScale;
  float negRadiusScale;
  float searchRadiusScale;
  passert(xmlconf.get("posRadiusScale", posRadiusScale));
  passert(xmlconf.get("negRadiusScale", negRadiusScale));
  passert(xmlconf.get("searchRadiusScale", searchRadiusScale));
  _posRadiusScaleBigger = _posRadiusScaleSmaller = posRadiusScale;
  _negRadiusScaleBigger = _negRadiusScaleSmaller = negRadiusScale;
  _searchRadiusScaleBigger = _searchRadiusScaleSmaller = searchRadiusScale;
  //passert(xmlconf.get("posRadiusScaleBigger", _posRadiusScaleBigger));
  //passert(xmlconf.get("posRadiusScaleSmaller", _posRadiusScaleSmaller));
  passert(xmlconf.get("negRadius", _negRadius));
  //passert(xmlconf.get("negRadiusScaleBigger", _negRadiusScaleBigger));
  //passert(xmlconf.get("negRadiusScaleSmaller", _negRadiusScaleSmaller));
  passert(xmlconf.get("searchRadius", _searchRadius));
  //passert(xmlconf.get("searchRadiusScaleBigger", _searchRadiusScaleBigger));
  //passert(xmlconf.get("searchRadiusScaleSmaller", _searchRadiusScaleSmaller));
  passert(xmlconf.get("scaleStep", _scaleStep));
  passert(xmlconf.get("posMinOverlap", _posMinOverlap));
  passert(xmlconf.get("negMaxOverlap", _negMaxOverlap));

  passert(xmlconf.get("learnRate", _clfparams._lRate))
    _clfparams._lRate = 1.f - _clfparams._lRate;
  passert(xmlconf.get("nFeat", _clfparams._numFeat))
    passert(xmlconf.get("nSel", _clfparams._numSel))

    _fixedSize = false;
  if (xmlconf.get("fixedSizeHeight", _fixedHeight) & xmlconf.get("fixedSizeWidth", _fixedWidth))
    _fixedSize = true;
  passert(xmlconf.get("minhwRatio", _minhwRatio));
  passert(xmlconf.get("maxhwRatio", _maxhwRatio));

}

MilTracker::~MilTracker()
{
  //      _feat->deregisterObserver( this);
}

MilTracker::MilTracker(shared_ptr<vca::icf::IntegralFeatures> feat) :
_cnt(0), _feat(feat), clsf(feat)
{
  //     _feat->registerObserver( this);
}

MilTracker::MilTracker(shared_ptr<vca::icf::IntegralFeatures>  feat, MilTrackerParams& p, cv::Rect& bbox) :
_cnt(0), _feat(feat), clsf(feat)
{
  //     _feat->registerObserver( this);
  setup(p);
  init(bbox);
}


//void MilTracker::process( vector< cv::Mat >& integralMap )
//{
//  clsf.updateFtrPointer( integralMap );
//}

cv::Rect MilTracker::getTrackBox()
{
  cv::Rect tmp;
  //    tmp = _curState;
  tmp.x = _curX - _feat->border();
  tmp.y = _curY - _feat->border();
  tmp.height = static_cast<int>(_trparams._height * _curScale);
  float dx = _trparams._height * _curScale / _trparams._hwRatio - _trparams._width * _curScale;
  tmp.width = static_cast<int>(_trparams._height * _curScale / _trparams._hwRatio);
  tmp.x -= static_cast<int>(dx * 0.5f + 0.5f);
  return tmp;
}

void initMinMaxScale(float curScale, int curX, int curY, int objwidth, int objheight, int imgwidth, int imgheight, float maxOverlap, float& minScale, float& maxScale)
{
  float minPixel = 10.f;

  minScale = max(minPixel / objheight, minPixel / objwidth);

  float maxWidth = (imgwidth - 3) / (3.f - 2.f*maxOverlap);
  float maxHeight = (imgheight - 3) / (3.f - 2.f*maxOverlap);
  maxScale = max(maxHeight / objheight, maxWidth / objwidth);
  passert(minScale <= maxScale);
}
bool MilTracker::init(cv::Rect& bbox)
{
  if (!_trparams._fixedSize)
  {
    //_trparams._width = static_cast<int>(bbox.width);
    //_trparams._height = static_cast<int>(bbox.height);
    //////////////////
    if (bbox.width <= bbox.height)
    {
      _trparams._width = static_cast<int>(bbox.height);
      _trparams._height = static_cast<int>(bbox.height);
    }
    else
    {
      _trparams._width = static_cast<int>(bbox.width);
      _trparams._height = static_cast<int>(bbox.width);
    }
  }
  else
  {
    sassert(_trparams._fixedWidth > 0);
    sassert(_trparams._fixedHeight > 0);
    _trparams._width = _trparams._fixedWidth;
    _trparams._height = _trparams._fixedHeight;
  }
  _trparams._hwRatio = static_cast<float>(bbox.height) / bbox.width;
  if (_trparams._minhwRatio > 0 && _trparams._hwRatio < _trparams._minhwRatio)
    _trparams._hwRatio = _trparams._minhwRatio;
  if (_trparams._maxhwRatio > 0 && _trparams._hwRatio > _trparams._maxhwRatio)
    _trparams._hwRatio = _trparams._maxhwRatio;
  _ftrParams._height = _trparams._height;
  _ftrParams._width = _trparams._width;
  _ftrParams._numCh = min(static_cast<int>(_feat->size()), _ftrParams._numCh);
  _ftrParams._paddingFactor = 0.f;
  //TODO testing color only: 
  // _ftrParams._numCh = 3;

  clsf.init(_trparams._clfparams, _ftrParams, _feat->getFeaturePipeType());
  _feat->update();
  _feat->updateFeatureChannelPointer();
  if (_ftrParams._numCh < 1)
    _ftrParams._numCh = _feat->size();
  //clsf.updateFtrPointer( _feat->intImgs_ );

  int imgwidth = _feat->width();
  int imgheight = _feat->height();

  if (bbox.width <= bbox.height)
  {
    _curScale = static_cast<float>(bbox.height) / _trparams._height;
    _curX = bbox.x - 0.5*(_trparams._width - bbox.width);
    _curY = bbox.y ;
  }
  else
  {
    _curScale = static_cast<float>(bbox.width) / _trparams._width;
    _curX = bbox.x;
    _curY = bbox.y - 0.5*(_trparams._height - bbox.height);
  }

  //_curScale = static_cast<float>(bbox.height) / _trparams._height;
  ////  _curState = bbox;
  //_curX = bbox.x + _feat->border();
  //_curY = bbox.y + _feat->border();

  initMinMaxScale(_curScale, _curX, _curY, _trparams._width, _trparams._height, imgwidth, imgheight, _trparams._negMaxOverlap, _trparams._minscale, _trparams._maxscale);

  _posx.clear();
  _negx.clear();

  // sample positives and negatives from first frame
  generateSamples(_posx, _posFtrValues, _curX, _curY, _curScale, _trparams._width, _trparams._height, _feat->width(), _feat->height(),
    _trparams._posRadius, _trparams._posRadiusScaleSmaller, _trparams._posRadiusScaleBigger, _trparams._posMinOverlap, 1.1f, _trparams._posmaxtrain,
    _trparams._minscale, _trparams._maxscale);
  //sampleImage(_posx, _posFtrValues, _curX, _curY, _curScale, _trparams._width, _trparams._height, _feat->width(), _feat->height(),
  //  _trparams._posRadius, _trparams._posRadiusScaleSmaller, _trparams._posRadiusScaleBigger, _trparams._posMinOverlap, 1.1f, _trparams._posmaxtrain,
  //  _trparams._minscale, _trparams._maxscale);


  _posFtrValues.clear();
  //selectSamples(_posFtrValues, _posx);
  clsf.computeFtr(*_feat, _posFtrValues, _posx, false);

  sampleImage(_negx, _negFtrValues, _curX, _curY, _curScale, _trparams._width, _trparams._height, _feat->width(), _feat->height(),
    _trparams._negRadius, _trparams._negRadiusScaleSmaller, _trparams._negRadiusScaleBigger, -0.1f, _trparams._negMaxOverlap, _trparams._init_negnumtrain,
    _trparams._minscale, _trparams._maxscale);


  _negFtrValues.clear();
  clsf.computeFtr(*_feat, _negFtrValues, _negx, false);

  if (_posx.size() < 1 || _negx.size() < 1)
    return false;

  
  //for (int r = 0; r < _posx.size(); r++)
  //{
  //  cout << _posx[r]._x << " - " << _posx[r]._y << endl;
  //}

  // train
  clsf.update(_posFtrValues, _negFtrValues);

  _cnt = 1;
  return true;
}

void MilTracker::logger(cv::Mat img, string folder, int frameNr)
{
  string path = "D:\\Experiments\\" + folder + "\\frame" + static_cast<ostringstream*>(&(ostringstream() << frameNr))->str() + "\\";
  cout << path << endl;

  //Logging Sigma Square, Mu and features
  cv::Rect curState;
  curState.x = _curX - _feat->border();
  curState.y = _curY - _feat->border();
  curState.width = static_cast<int>(_trparams._width * _curScale);
  curState.height = static_cast<int>(_trparams._height * _curScale);

  if (curState.x >= 0 && curState.y >= 0 && curState.x + curState.width < img.cols && curState.y + curState.height < img.rows)
    cv::imwrite("D:\\Experiments\\" + folder + "\\groundTruth" + static_cast<ostringstream*>(&(ostringstream() << frameNr))->str() + ".png", img(curState));
    clsf.logAll(path, img(curState), _curScale);

  //Logging positive and negative feature values
  ofstream posLogFile(path + "posSamples.txt", ios::app);
  ofstream negLogFile(path + "negSamples.txt", ios::app);

  if (posLogFile.is_open())
  {
    for (int m = 0; m < _posFtrValues.size(); m++)
    {
      for (int n = 0; n < _posFtrValues[0].cols; n++)
      {
        posLogFile << _posFtrValues[m].at<float>(0, n) << " ";
      }
      posLogFile << " \n";
    }
    posLogFile.close();
  }
  else cout << "Unable to open posLogFile";

  if (negLogFile.is_open())
  {
    for (int m = 0; m < _negFtrValues.size(); m++)
    {
      for (int n = 0; n < _negFtrValues[0].cols; n++)
      {
        negLogFile << _negFtrValues[m].at<float>(0, n) << " ";
      }
      negLogFile << "\n";
    }
    negLogFile.close();
  }
  else cout << "Unable to open negLogFile";

  //Logging the frame after segmentation
  if (_segmentMask.rows != 0 && _segmentMask.cols != 0)
    cv::imwrite("D:\\Experiments\\" + folder + "\\segmented" + static_cast<ostringstream*>(&(ostringstream() << frameNr))->str() + ".png", _segmentMask);
  else
    cout << "no segmentation" << endl;
}


void MilTracker::drawDebug(cv::Mat& img)
{
  //   cv::rectangle( img, cv::Rect(100,100,50,50), cv::Scalar( 255,0,255),4);
  for (auto k = 0; k < static_cast<int>(_sample.size()); k++)
  {
    auto prob = _sample[k]._prob;
    cv::Vec3b color = cv::Vec3b(0, static_cast<uchar>(std::max(0.f, min(2.5f * prob, 255.f))), static_cast<uchar>(std::max(0.f, min(-prob, 255.f))));
    auto x = _sample[k]._x - _feat->border() + static_cast<int>(_sample[k]._scale * _trparams._width) / 2;
    auto y = _sample[k]._y - _feat->border() + static_cast<int>(_sample[k]._scale * _trparams._height) / 2;
    if (x >= 0 && x < img.cols && y >= 0 && y < img.rows)
    {
      img.at<cv::Vec3b>(y, x) = color;
      //  cout << x << " " << y << endl;
    }
  }
}

float MilTracker::track_frame()
{
 // cout << "track_frame" << endl;

  // run current clf on search window
  _sample.clear();

  sampleImage(_sample, _sampleFtrValues, _curX, _curY, _curScale, _trparams._width, _trparams._height, _feat->width(), _feat->height(),
    _trparams._searchRadius, _trparams._searchRadiusScaleSmaller, _trparams._searchRadiusScaleBigger, -0.1f, 1.1f, _trparams._searchmax,
    _trparams._minscale, _trparams._maxscale);

  _sampleFtrValues.clear();
  clsf.computeFtr(*_feat, _sampleFtrValues, _sample, true);
  clsf.classify(_sample, _sampleFtrValues);

  sassert(_sample.size() > 0)

    // find best location
    //auto maxElemPtr = std::max_element(_sample.begin(), _sample.end(), [](Sample& x, Sample& y){ return x._prob*x._probpos < y._prob*y._probpos; });

    auto maxElemPtr = std::max_element(_sample.begin(), _sample.end(), [](Sample& x, Sample& y){ return x._prob < y._prob; });
  //   int bestind = static_cast<int> (maxElemPtr - sample.begin());

  _curY = (*maxElemPtr)._y;
  _curX = (*maxElemPtr)._x;
  _curScale = (*maxElemPtr)._scale;

  return (*maxElemPtr)._prob;
}

void MilTracker::track_GT_frame(cv::Rect currGT, float scale)
{

  //_trparams._width = static_cast<int>(currGT.width);
  //_trparams._height = static_cast<int>(currGT.height);

  //_trparams._hwRatio = static_cast<float>(currGT.height) / currGT.width;

  clsf.genNewFtr(_trparams._clfparams, _ftrParams, _feat->getFeaturePipeType());
  _feat->update();
  _feat->updateFeatureChannelPointer();

  if (currGT.width <= currGT.height)
  {
    _curX = currGT.x - 0.5*(_trparams._width - currGT.width);
    _curY = currGT.y;
  }
  else
  {
    _curX = currGT.x;
    _curY = currGT.y - 0.5*(_trparams._height - currGT.height);
  }
  //_curX = currGT.x;
  //_curY = currGT.y;

  //cout << "width scale: " << (float) currGT.width / prevGT.width << endl;
  //cout << "height scale: " << (float) currGT.height / prevGT.height << endl;
  //cout << _trparams._hwRatio << endl;

  _curScale = 1;// scale;

  _sample.clear();

  sampleImage(_sample, _sampleFtrValues, _curX, _curY, _curScale, _trparams._width, _trparams._height, _feat->width(), _feat->height(),
    _trparams._searchRadius, _trparams._searchRadiusScaleSmaller, _trparams._searchRadiusScaleBigger, -0.1f, 1.1f, _trparams._searchmax,
    _trparams._minscale, _trparams._maxscale);

  _sampleFtrValues.clear();
  clsf.computeFtr(*_feat, _sampleFtrValues, _sample, true);
  clsf.classify(_sample, _sampleFtrValues);

  sassert(_sample.size() > 0)

    // find best location
    //auto maxElemPtr = std::max_element(_sample.begin(), _sample.end(), [](Sample& x, Sample& y){ return x._prob*x._probpos < y._prob*y._probpos; });

    auto maxElemPtr = std::max_element(_sample.begin(), _sample.end(), [](Sample& x, Sample& y){ return x._prob < y._prob; });
  //   int bestind = static_cast<int> (maxElemPtr - sample.begin());

  //_curY = (*maxElemPtr)._y;
  //_curX = (*maxElemPtr)._x;
  //_curScale = (*maxElemPtr)._scale;
  //cout << _sample.size() << endl;
  //for(int r = 0; r < _sample.size(); r++)
  //  cout << _sample[r]._prob << "    " << _curY << "  " << _curX << "   " << _curScale << endl;

}

void MilTracker::updateClf()// string folder, int frameNum)
{
  //cout << "updateClf" << endl;

  // train location clf (negx are randomly selected from image, posx is just the current tracker location)
  _posx.clear();
  _negx.clear();

  sampleImage(_posx, _posFtrValues, _curX, _curY, _curScale, _trparams._width, _trparams._height, _feat->width(), _feat->height(),
    _trparams._posRadius, _trparams._posRadiusScaleSmaller, _trparams._posRadiusScaleBigger, _trparams._posMinOverlap, 1.1f, _trparams._posmaxtrain,
    _trparams._minscale, _trparams._maxscale);


  _posFtrValues.clear();
  clsf.computeFtr(*_feat, _posFtrValues, _posx, false);

  sampleImage(_negx, _negFtrValues, _curX, _curY, _curScale, _trparams._width, _trparams._height, _feat->width(), _feat->height(),
    _trparams._negRadius, _trparams._negRadiusScaleSmaller, _trparams._negRadiusScaleBigger, -0.1f, _trparams._negMaxOverlap, _trparams._negnumtrain,
    _trparams._minscale, _trparams._maxscale);

  _negFtrValues.clear();
  clsf.computeFtr(*_feat, _negFtrValues, _negx, false);

  sassert(_posx.size() > 0);
  sassert(_negx.size() > 0);

  //selectSamples(_posFtrValues, _posx);
  // cout << _posx.size() << " " << _negx.size() << " " << _trparams._negnumtrain << endl;
  //cout << "posSamples: " << _posFtrValues[0] << endl;
  //cout << "negSamples: " << _negFtrValues[0] << endl;
  clsf.update(_posFtrValues, _negFtrValues);// , folder, frameNum);

  _cnt++;

}

void MilTracker::segmentDetBox(cv::Mat& img, cv::Rect trackBox)
{
  //Getting current track box
  //cv::Rect trackBox = getTrackBox();
  int prob = 0;
  int x1 = trackBox.x;
  int y1 = trackBox.y;
  int x2 = (trackBox.x + trackBox.width);
  int y2 = (trackBox.y + trackBox.height);

  //cout << "x: " << trackBox.x << "  y: " << trackBox.y << "  width: " << trackBox.width << " height: " << trackBox.height << endl;

  std::vector<cv::Mat> imgChannels;
  //cv::Mat ra[3] = {im1, im2, im3};
  cv::split(img, imgChannels);

  cv::Mat fgWeight(img.rows, img.cols, CV_64FC1);
  cv::Mat bgWeight(img.rows, img.cols, CV_64FC1);

  std::pair<cv::Mat, cv::Mat> probs;

  if (prob > 0){
    fgWeight.setTo(0.0);
    //weights are epanechnikov distr. with peek at the center of the image;
    double cx = x1 + (x2 - x1) / 2.;
    double cy = y1 + (y2 - y1) / 2.;
    double kernelSize_width = 1.0 / (0.5*static_cast<double>(x2 - x1)*1.4142 + 1);  //sqrt(2)
    double kernelSize_height = 1.0 / (0.5*static_cast<double>(y2 - y1)*1.4142 + 1);

    for (int y = y1; y < y2 + 1; ++y){
      double * weightPtr = fgWeight.ptr<double>(y);
      double tmp_y = std::pow((cy - y)*kernelSize_height, 2);
      for (int x = x1; x < x2 + 1; ++x){
        weightPtr[x] = kernelProfile_Epanechnikov(std::pow((cx - x)*kernelSize_width, 2) + tmp_y);
      }
    }
    bgWeight = cv::Mat::ones(img.rows, img.cols, CV_64FC1) - fgWeight;

    cv::imshow("fg weight", fgWeight);
    cv::waitKey(0);

    probs = FSeg::Segment::computePosteriors(imgChannels, x1, y1, x2, y2, cv::Mat(), fgWeight, bgWeight, FSeg::Segment::Histogram(), 8);
  }
  else{
    probs = FSeg::Segment::computePosteriors(imgChannels, x1, y1, x2, y2, cv::Mat(), cv::Mat(), cv::Mat(), FSeg::Segment::Histogram(), 8);
  }

  cv::Mat mask(probs.first.rows, probs.first.cols, CV_8UC1);
  probs.first.convertTo(probs.first, CV_32F);
  probs.second.convertTo(probs.second, CV_32F);

  cv::Mat responseOutput = probs.first.clone();
  double maxScaleResponse;
  minMaxLoc(responseOutput, 0, &maxScaleResponse, 0, 0);

  responseOutput = responseOutput / maxScaleResponse *255.f;
  responseOutput.convertTo(responseOutput, CV_8UC1);
  cv::Mat cm_img0;
  // Apply the colormap:
  applyColorMap(responseOutput, cm_img0, cv::COLORMAP_JET);
  //cv::imshow("probfg", cm_img0);
  // cv::waitKey(0);

  cv::threshold(probs.first / probs.second, mask, 1, 255, cv::THRESH_BINARY);
  cv::Mat wholeMask(img.rows, img.cols, CV_8UC1);
  wholeMask.setTo(0);
  mask.copyTo(wholeMask(cv::Rect(x1, y1, x2 - x1, y2 - y1)));
  cv::Mat res;
  cv::bitwise_not(wholeMask, res);
  res.copyTo(_segmentMask);

  cv::Mat wholeMask2(img.rows, img.cols, CV_8UC1);
  wholeMask2.setTo(0);
  cv::threshold(probs.second / probs.first, mask, 1, 255, cv::THRESH_BINARY);
  mask.copyTo(wholeMask2(cv::Rect(x1, y1, x2 - x1, y2 - y1)));

  //imgChannels[2] = 0.7*wholeMask + imgChannels[2];
  //imgChannels[0] = 0.7*wholeMask2 + imgChannels[0];
  //cv::merge(imgChannels, img);

  //cv::imshow("foreground Mask", res);
  //cv::imshow("background Mask", wholeMask2);
  //cv::imshow("img", img);
  //cv::imshow("Background posterior", probs.second);
  //cv::waitKey(0);

}

void MilTracker::selectSamples(vector<cv::Mat>&sampleFtrValues, vector< Sample >& samples)
{
  //cout << "drawFeatures" << endl;

  cv::Rect curState;
  curState.x = _curX - _feat->border();
  curState.y = _curY -_feat->border();
  curState.width = static_cast<int>(_trparams._width * _curScale);
  curState.height = static_cast<int>(_trparams._height * _curScale);
  //cv::imshow("Object", _segmentMask);
  //if (curState.x >= 0 && curState.y >= 0 && curState.x + curState.width < _segmentMask.cols && curState.y + curState.height < _segmentMask.rows)
  clsf.preselect(_segmentMask(curState), _curScale);

}


void MilTracker::logSampleValues(std::vector<cv::Mat>& posFtrValues, std::vector<cv::Mat>& negFtrValues, string folder, int frameNum)
{
  //cout << frameNum << endl;
}

double MilTracker::kernelProfile_Epanechnikov(double x)
{
  return (x <= 1) ? (2.0 / 3.14)*(1 - x) : 0;
}


float MilTracker::classify(int x, int y, int w, int h)
{
  auto trackerW = _trparams._width * h / static_cast<float>(_trparams._height);
  auto dx = static_cast<int>((w - trackerW + 1.f)*0.5f);

  x += _feat->border() + dx;
  y += _feat->border();
  float scale = static_cast<float>(h) / _trparams._height;


  _clfsample.clear();
  sampleImage(_clfsample, _clfsampleFtrValues, x, y, scale, _trparams._width, _trparams._height, _feat->width(), _feat->height(),
    _trparams._posRadius, _trparams._posRadiusScaleSmaller, _trparams._posRadiusScaleBigger, _trparams._posMinOverlap, 1.1f, _trparams._posmaxtrain,
    _trparams._minscale, _trparams._maxscale);
  sassert(_clfsample.size() > 0);
  _clfsampleFtrValues.clear();
  clsf.computeFtr(*_feat, _clfsampleFtrValues, _clfsample, true);
  sassert(_clfsample.size() > 0);

  clsf.classify(_clfsample, _clfsampleFtrValues);
  sassert(_clfsample.size() > 0);

  //  auto maxElemPtr = std::max_element(_clfsample.begin(), _clfsample.end(), [](Sample& x, Sample& y){ return x._prob*x._probpos < y._prob*y._probpos; });
  auto maxElemPtr = std::max_element(_clfsample.begin(), _clfsample.end(), [](Sample& x, Sample& y){ return x._prob < y._prob; });
  sassert(maxElemPtr != _clfsample.end());
  return (*maxElemPtr)._prob;

}


void MilTracker::updateClf(int x, int y, int w, int h, float trust)
{
  sassert(trust <= 1.f);
  sassert(trust >= 0.f);
  _curX = (1.f - trust)*_curX + trust*(x + _feat->border());
  _curY = (1.f - trust)*_curY + trust*(y + _feat->border());
  _curScale = (1.f - trust)*_curScale + trust*static_cast<float>(h) / _trparams._height;
  _trparams._hwRatio = (1.f - trust)*_trparams._hwRatio + trust*static_cast<float>(h) / w;
  if (_trparams._minhwRatio > 0 && _trparams._hwRatio < _trparams._minhwRatio)
    _trparams._hwRatio = _trparams._minhwRatio;
  if (_trparams._maxhwRatio > 0 && _trparams._hwRatio > _trparams._maxhwRatio)
    _trparams._hwRatio = _trparams._maxhwRatio;
  auto dx = (w - h / (static_cast<float>(_trparams._height) / _trparams._width))*0.5f;
  _curX += static_cast<int>(dx + 0.5f);
  //updateClf();
}

void MilTracker::drawFeatures(cv::Mat& img)// , string folder, int frameNum)
{
  //cout << "drawFeatures" << endl;
  cv::Rect curState;
  curState.x = _curX -_feat->border();
  curState.y = _curY -_feat->border();
  curState.width = static_cast<int>(_trparams._width * _curScale);
  curState.height = static_cast<int>(_trparams._height * _curScale);

  if (curState.x >= 0 && curState.y >= 0 && curState.x + curState.width < img.cols && curState.y + curState.height < img.rows)
    clsf.drawFeatures(img(curState), _curScale);// , folder, frameNum);
}

void MilTracker::drawPosSamples(cv::Mat& img)
{
  //cout << "drawPosSamples" << endl;
  drawSamples(_posx, img, cv::Scalar(0, 255, 0));
}

void MilTracker::drawNegSamples(cv::Mat& img)
{
 // cout << "drawNegSamples" << endl;
  drawSamples(_negx, img, cv::Scalar(0, 0, 255));
}

void MilTracker::drawSamples(const std::vector<Sample>& samples, cv::Mat& img, cv::Scalar color)
{
  for (auto i = 0; i < samples.size(); ++i)
  {
    cv::Rect roi;// = samples[i]._roi;
    roi.x = samples[i]._x - _feat->border();
    roi.y = samples[i]._y - _feat->border();
    roi.width = static_cast<int>(samples[i]._scale * _trparams._width);
    roi.height = static_cast<int>(samples[i]._scale * _trparams._height);
    cv::rectangle(img, roi, color, 1);

  }
}

void MilTracker::setup(Vca::XmlConf& xmlconf)
{
  MilTrackerParams p(xmlconf);
  setup(p);

}

void MilTracker::setup(MilTrackerParams& p)
{
  _trparams = p;
}

void MilTracker::generateSamples(std::vector<Sample>& samples, std::vector<cv::Mat>& sampleFtrValues, int x, int y, float scale, int width, int height, int ncols, int nrows,
  float maxRadPixelRatio, float minScaleChange, float maxScaleChange, float minOverlap, float maxOverlap, int maxnum, float globalMinScale, float globalMaxScale)
{

}

//void MilTracker::sampleImage(std::vector<Sample>& samples, std::vector<cv::Mat>& sampleFtrValues, int x, int y, float scale, int width, int height, int ncols, int nrows,
//  float inrad, float inradscale, bool scaleIndipendent /*= false*/, float outrad /*= 0.f*/, float outradscale /*= 0.f*/, int maxnum /*= 1000000*/)
void MilTracker::sampleImage(std::vector<Sample>& samples, std::vector<cv::Mat>& sampleFtrValues, int x, int y, float scale, int width, int height, int ncols, int nrows,
  float maxRadPixelRatio, float minScaleChange, float maxScaleChange, float minOverlap, float maxOverlap, int maxnum, float globalMinScale, float globalMaxScale)
{

  auto scalestepsMin = static_cast<int>(minScaleChange / _trparams._scaleStep);
  auto scalestepsMax = static_cast<int>(maxScaleChange / _trparams._scaleStep);

  int sheight = static_cast<int>(height * scale);
  int swidth = static_cast<int>(width * scale);

  float maxRadPixel = 0.5f*(height + width) * maxRadPixelRatio*scale;

  if (maxRadPixel < 1.f)
    maxRadPixel = 1.f;
  float radPixelsq = maxRadPixel * maxRadPixel;


  float minScale = scale - minScaleChange;
  float maxScale = scale + maxScaleChange;
  if (minScale  < globalMinScale)
  {
    minScale = globalMinScale;
  }

  if (maxScale  > globalMaxScale)
  {
    maxScale = globalMaxScale;
  }


  float centerscale = scale;
  if (maxScale < scale)
    centerscale = maxScale - 0.00001f;
  sassert(minScale <= scale);
  if (maxScale < minScale)
  {
    maxScale = centerscale + 0.00001f;
    minScale = centerscale - 0.00001f;
  }
  sassert(minScale <= maxScale)


    //auto volumn1 = static_cast<float>(2.f / 3.f*M_PI*(radPixelsq*scalestepsMin));
    //auto volumn2 = static_cast<float>(2.f / 3.f*M_PI*(radPixelsq*(scalestepsMax-1)));
    //auto volumn = volumn1 + volumn2;

    // auto area = static_cast<int>( 0.25f*M_PI* ((maxrow - minrow + 1) * (maxcol - mincol + 1) - (maxrowout - minrowout + 1) * (maxcolout - mincolout + 1)));
    //auto area = static_cast<float>( M_PI*(inradsq - outradsq));
    //   auto volumn = static_cast<float>(4.f / 3.f*M_PI*(inradsq*insteps - outradsq*outsteps));
    //   auto volumn = static_cast<float>(2.f / 3.f*M_PI*(inradsq*insteps - outradsq*outsteps));
    //   auto volumn = static_cast<float>(M_PI*(inradsq - outradsq));
    samples.clear();
  std::vector<Sample> allsamples;
  // samples.reserve( maxnum );

  // float prob = (static_cast<float> (maxnum)) /( volumn);
  //   float sumprob = 0;
  //  int cntpxl = 0;

  //  cout << scale << " " << minScale << " " << centerscale << " " << maxScale << " " << globalMinScale << " " << globalMaxScale<< endl;
  for (auto s = centerscale; s <= maxScale; s += _trparams._scaleStep)
  {
    int rowsz = nrows - static_cast<int>(height * s) - 1;
    int colsz = ncols - static_cast<int>(width * s) - 1;
    int minrow = std::max(0, y - static_cast<int>(maxRadPixel));
    int maxrow = std::min(rowsz - 1, y + static_cast<int>(maxRadPixel));
    int mincol = std::max(0, x - static_cast<int>(maxRadPixel));
    int maxcol = std::min(colsz - 1, x + static_cast<int>(maxRadPixel));
    for (int r = minrow; r <= maxrow; r++)
    {
      for (int c = mincol; c <= maxcol; c++)
      {
        float d1 = ((y - r) * (y - r) + (x - c) * (x - c)) / radPixelsq;
        if (maxScaleChange > 0)
          d1 += (scale - s)*(scale - s) / (maxScaleChange*maxScaleChange);
        if (d1 <= 1.f)
        {

          int sy = static_cast<int>(r + (height * scale - s * height)*.5f);
          int sx = static_cast<int>(c + (width * scale - s * width)*.5f);

          float xoverlap = min(sx + s * width, x + width * scale)
            - max(sx, x);
          float yoverlap = min(sy + s * height, y + height * scale)
            - max(sy, y);
          float overlap = 0.f;
          if (xoverlap > 0.f && yoverlap > 0.f)
          {
            overlap = xoverlap * yoverlap;
          }
          overlap /= (s * width*s * height + width * scale*height * scale - overlap);
          if (sx >= 0 && sy >= 0 && sx + s * width < ncols && sy + s * height < nrows && overlap >= minOverlap && overlap <= maxOverlap)
            allsamples.emplace_back(Sample(sx, sy, s, ncols, 1.f - d1, 0.f));

        }
      }
    }
    sassert(allsamples.size() > 0);
  }

  for (auto s = minScale; s + 0.00001 < scale; s += _trparams._scaleStep)
  {
    int rowsz = nrows - static_cast<int>(height * s) - 1;
    int colsz = ncols - static_cast<int>(width * s) - 1;
    int minrow = std::max(0, y - static_cast<int>(radPixelsq));
    int maxrow = std::min(rowsz - 1, y + static_cast<int>(radPixelsq));
    int mincol = std::max(0, x - static_cast<int>(radPixelsq));
    int maxcol = std::min(colsz - 1, x + static_cast<int>(radPixelsq));
    for (int r = minrow; r <= maxrow; r++)
    {
      for (int c = mincol; c <= maxcol; c++)
      {
        sassert(minScaleChange >0);
        float d1 = ((y - r) * (y - r) + (x - c) * (x - c)) / radPixelsq + (scale - s)*(scale - s) / (minScaleChange*minScaleChange);
        if (d1 <= 1.f)
        {
          int sy = static_cast<int>(r + (height * scale - s * height)*.5f);
          int sx = static_cast<int>(c + (width * scale - s * width)*.5f);

          float xoverlap = min(sx + s * width, x + width * scale)
            - max(sx, x);
          float yoverlap = min(sy + s * height, y + height * scale)
            - max(sy, y);
          float overlap = 0.f;
          if (xoverlap > 0.f && yoverlap > 0.f)
          {
            overlap = xoverlap * yoverlap;
          }
          overlap /= (s * width*s * height + width * scale*height * scale - overlap);
          if (sx >= 0 && sy >= 0 && sx + s * width < ncols && sy + s * height < nrows && overlap >= minOverlap && overlap <= maxOverlap)
            allsamples.emplace_back(Sample(sx, sy, s, ncols, 1.f - d1, 0.f));
        }
      }
    }
  }

  float prob = static_cast<float>(maxnum) / allsamples.size();
  float cntprob = 0.f;
  for (int i = 0; i < allsamples.size(); ++i)
  {
    cntprob += prob;
    if (cntprob > samples.size() + 1)
      samples.push_back(allsamples[i]);
  }

  sassert(samples.size() > 0);
}

int MilTracker::getCount()
{
  return _cnt;
}
