#pragma once
#include <string>
#include "ClfOnlineStump.h"
#include "features\Icf\IntegralFeatures.h"
#include "features/Icf/FeatureContainer.h"

#include "tracking/TrackingLogger.h"
#include "tracking/trackingMacros.h"


class VCA_API ClfMilBoostParams
{
public:
  ClfMilBoostParams(){}
  ClfMilBoostParams(vector<int> numFeat, int numSel, int numFeatSize, float lRate);
public:
  vector<int> _numFeat;
  int _numSel;
  float _lRate; // learning rate for weak learners;
};

class VCA_API HaarFtrParams
{
public:
  HaarFtrParams();
  int _numCh;
  int _width, _height;
  float _paddingFactor;
};



//////////////////////////////////////////////////////////////////////////////////////////////////////////

class VCA_API ClfMilBoost
{

public:

  ClfMilBoost(shared_ptr<vca::icf::IntegralFeatures> ift) : _featCon(ift){}


  ClfMilBoost(shared_ptr<vca::icf::IntegralFeatures> ift,
    ClfMilBoostParams& clfparams, HaarFtrParams& ftrParams);

  // int nFtrs();
  void init(ClfMilBoostParams& params, HaarFtrParams& ftrParams, vca::icf::FeaturepipeType& fptype);


  void update(vector<cv::Mat>&posx, vector<cv::Mat>&negx);// , string folder, int frameNum);
  void classify(vector< Sample>& samples, vector<cv::Mat>& sampleFeatureValues);

  //static cv::Mat applyToImage(ClfMilBoost& clf, const std::vector<cv::Mat >& iimgs, int ncols, int nrows, bool logR = true); // returns a probability map (or log odds ratio map if logR=true)
  //static void eval(std::vector<float> ppos, std::vector<float> pneg, float &err, float &fp, float &fn, float thresh = 0.5f);
  //static float likl(std::vector<float> ppos, std::vector<float> pneg);

  // void updateFtrPointer( map<vca::Icf::ChannelType,cv::Mat>& integralImgs );

  void computeFtr(vca::icf::IntegralFeatures& icf, vector<cv::Mat>&sampleFtrValues, vector< Sample >& samples, bool onlySelected);

  void generateFtr(HaarFtrParams& ftrParams, vector<int>& num, vca::icf::FeaturepipeType& fptype, vca::icf::FeatureContainer& _featCon);

  void genNewFtr(ClfMilBoostParams& params, HaarFtrParams& ftrParams, vca::icf::FeaturepipeType& fptype);

  void drawFeatures(cv::Mat& img, float scale);//, string folder, int frameNumber);

  void preselect(cv::Mat& segmentMask, float scale);

  void logAll(string path, cv::Mat img, float scale);

public:


private:
  int _nSelector;
  // std::vector<basisFtr> _ftrs; 
  vca::icf::FeatureContainer _featCon;
  std::vector<int> _selectors;
  std::vector<ClfOnlineStump> _weakclf;


  vector<vector<ClfOnlineStump::stumpResult>> _pospred;
  vector<vector<ClfOnlineStump::stumpResult>> _negpred;
  vector<float> _poslikl;
  vector<float> _neglikl;
  vector<float> _liklv;
  vector<int> _order;

};

////////////////////////////////////////////////////////////////////////////////////////////