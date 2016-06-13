#pragma once
#include "sample.h"
#include "milutil.h"

#include "tracking/TrackingLogger.h"
#include "tracking/trackingMacros.h"

class ClfMilBoost;

//////////////////////////////////////////////////////////////////////////////////////////////////////////
class  VCA_API ClfOnlineStump
{
public:
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  // members
  float _mu0, _mu1, _sigSq0, _sigSq1;
  float _log_n1, _log_n0;
  float _e1, _e0;
public:
  //////////////////////////////////////////////////////////////////////////////////////////////////////
  // functions
  ClfOnlineStump();
  ClfOnlineStump(int ftrIndex);


  struct stumpResult
  {
    stumpResult()
    {
      pos = 0.f;
      neg = 0.f;
    }

    stumpResult(float newPos, float newNeg)
    {
      pos = newPos;
      neg = newNeg;
    }

    stumpResult& operator+=(const stumpResult& rhs) // compound assignment (does not need to be a member,
    {                           // but often is, to modify the private members)
      /* addition of rhs to *this takes place here */
      pos += rhs.pos;
      neg += rhs.neg;
      return *this; // return the result by reference
    }


    float pos;

    float neg;
  };

  void init();
  void update(vector<cv::Mat>&posx, vector<cv::Mat>&negx, const cv::Mat_<float> & posw = cv::Mat_<float>(), const cv::Mat_<float>& negw = cv::Mat_<float>());
  //bool classify( SampleSet &x, int i);
  stumpResult classifyF(vector<cv::Mat>& sampleFtrValues, int sampleIdx);

  void classifySetF(vector<cv::Mat> &x, std::vector<stumpResult>& result);

  // float getFtrVal( const SampleSet &x, int i);

protected:
  bool _trained;
  //   basisFtr *_ftr;
  int _ftrIndex;
  float _lRate;

  friend class ClfMilBoost;

};


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//inline std::vector<bool> ClfOnlineStump::classifySet( SampleSet &x)
//{
//  std::vector<bool> res(x.size());
//  for (auto k = 0; k < (int) res.size(); k++)
//  {
//    res[k] = classify( x, k);
//  }
//  return res;
//}
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//inline bool ClfOnlineStump::classify( SampleSet &x, int i)
//{
//  float xx = getFtrVal( x, i);
//  double log_p0 = (xx - _mu0) * (xx - _mu0) * _e0 + _log_n0;
//  double log_p1 = (xx - _mu1) * (xx - _mu1) * _e1 + _log_n1;
//  return log_p1 > log_p0;
//}
