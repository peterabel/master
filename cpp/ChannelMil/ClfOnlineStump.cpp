#include "ClfOnlineStump.h"

ClfOnlineStump::ClfOnlineStump()
{
  _trained = false;
  _ftrIndex = -1;
  init();
}

ClfOnlineStump::ClfOnlineStump(int id)
{
  _trained = false;
  _ftrIndex = id;
  init();
}


void ClfOnlineStump::init()
{
  _mu0 = 0;
  _mu1 = 0;
  _sigSq0 = 1;
  _sigSq1 = 1;
  _lRate = 0.85f;
  _trained = false;
}

ClfOnlineStump::stumpResult ClfOnlineStump::classifyF(vector<cv::Mat>& sampleFtrValues, int sampleIdx)
{
  float xx = sampleFtrValues[_ftrIndex].at<float>(sampleIdx);
  double log_p0 = (xx - _mu0) * (xx - _mu0) * _e0 + _log_n0;
  double log_p1 = (xx - _mu1) * (xx - _mu1) * _e1 + _log_n1;
  return stumpResult( log_p1, log_p0 );
}

void ClfOnlineStump::update(vector<cv::Mat>&posx, vector<cv::Mat>&negx, const cv::Mat_<float> & posw /*= cv::Mat_<float>()*/, const cv::Mat_<float>& negw /*= cv::Mat_<float>()*/)
{
  float posmu = 0.0, negmu = 0.0;
  if (posx.size() > 0)
    posmu = (float)cv::mean(posx[_ftrIndex])[0];
    if (negx.size() > 0)
    negmu = (float)cv::mean(negx[_ftrIndex])[0];

  if (_trained)
  {
    if (posx.size() > 0)
    {
      _mu1 = (_lRate * _mu1 + (1 - _lRate) * posmu);
      cv::Mat diff = posx[_ftrIndex] - _mu1;
      _sigSq1 = (float)(_lRate * _sigSq1 + (1 - _lRate) * cv::mean(diff.mul(diff))[0]);
    }
    if (negx.size() > 0)
    {
      _mu0 = (_lRate * _mu0 + (1 - _lRate) * negmu);
      cv::Mat diff = negx[_ftrIndex] - _mu0;
      _sigSq0 = (float)(_lRate * _sigSq0 + (1 - _lRate) * cv::mean(diff.mul(diff))[0]);
    }
  }
  else
  {
    _trained = true;
    if (posx.size() > 0)
    {
      _mu1 = posmu;
      cv::Scalar scal_mean, scal_std_dev;
      cv::meanStdDev(posx[_ftrIndex], scal_mean, scal_std_dev);
      _sigSq1 = (float)(scal_std_dev[0] * scal_std_dev[0] + 1e-9f);
    }

    if (negx.size() > 0)
    {
      _mu0 = negmu;
      cv::Scalar scal_mean, scal_std_dev;
      cv::meanStdDev(negx[_ftrIndex], scal_mean, scal_std_dev);
      _sigSq0 = (float)(scal_std_dev[0] * scal_std_dev[0] + 1e-9f);
    }
  }
  //cout << "mu0: " << _mu0 << " sigSq0: " << _sigSq0 << endl;
  //cout << "mu1: " << _mu1 << " sigSq1: " << _sigSq1 << endl;

  _log_n0 = -0.5f * std::log(_sigSq0);
  _log_n1 = -0.5f * std::log(_sigSq1); //constant term +log(1/sqrt(2*PI)) is negligible

  _e1 = -1.0f / (2.0f * _sigSq1 + std::numeric_limits<float>::min());
  _e0 = -1.0f / (2.0f * _sigSq0 + std::numeric_limits<float>::min());
}

void ClfOnlineStump::classifySetF(vector<cv::Mat> &x, std::vector<stumpResult>& result)
{
  result.resize(x[0].cols);

  //#pragma omp parallel for
  for (auto k = 0; k < (int)result.size(); k++)
  {
    result[k] = classifyF(x, k);
  }
}
