#include "ClfMilBoost.h"
#include <numeric>
#include "features\Icf\BoxFeatureFactory.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2\imgproc.hpp"
using namespace std;



ClfMilBoostParams::ClfMilBoostParams(vector<int> numFeat, int numSel, int numFeatSize, float lRate)
:
_lRate(lRate),
_numSel(numSel),
_numFeat(numFeat)
{}

HaarFtrParams::HaarFtrParams()
{
  _numCh = -1;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////


ClfMilBoost::ClfMilBoost(shared_ptr<vca::icf::IntegralFeatures> ift,
  ClfMilBoostParams& params, HaarFtrParams& ftrParams)
  : _featCon(ift)
{
  init(params, ftrParams, ift->getFeaturePipeType());
}


void ClfMilBoost::computeFtr(vca::icf::IntegralFeatures& icf, vector<cv::Mat>&sampleFtrValues, vector< Sample >& samples, bool onlySelected)
{

  auto numftrs = _featCon.numBoxes();
  auto numsamples = samples.size();
  if (numsamples == 0)
    return;

  //  cout << numftrs << endl;
  sampleFtrValues.clear();
  //   sampleFtrValues.resize( numftrs );
  for (auto i = 0; i < numftrs; ++i)
  {
    sampleFtrValues.emplace_back(cv::Mat::zeros(1, numsamples, CV_32F));
  }

  int maxF = _featCon.numBoxes();
  if (onlySelected)
    maxF = _selectors.size();
  //cout << maxF << endl;
  #pragma omp parallel for
  for (auto w = 0; w < maxF; w++)
  {
    int ftr = w;
    if (onlySelected)
      ftr = _selectors[w];
    //cout << ftr << endl;
    for (int k = 0; k < numsamples; k++)
    {
      //auto tmpftr = _ftrs[ftr];
      //float featurescaleFactor =  _ftrs[ftr].scaleFeatBox( (icf), tmpftr, samples[ k ]._scale );
      //  tmpftr.set(integralImgs[_ftrs[ ftr ].idx],0,0);
      //float tmp =  tmpftr.eval(  samples[k]._offset )/featurescaleFactor;
      auto tmp = _featCon.evalScaledBox(ftr, samples[k]._scale, samples[k]._offset);
      sampleFtrValues[ftr].at<float>(k) = tmp;
    }
  }
}

//void ClfMilBoost::generateFtr( HaarFtrParams& ftrParams, int num, std::vector<basisFtr>& result)
//{
//  //auto paddingFactor = 0.25f;
//  auto paddingFactor = 0.1f;
//  auto paddingPixelHeight = static_cast<int>(paddingFactor * ftrParams._height);
//  auto paddingPixelWidth = static_cast<int>(paddingFactor * ftrParams._width);
//  auto height = ftrParams._height - 2 * paddingPixelHeight;
//  auto width = ftrParams._width - 2 * paddingPixelWidth;

//  result.clear();
//  auto numPerChannel = num / ftrParams._numCh;
//  auto heightWidthRatio = static_cast<float>(height)/width;
//  auto nHeight = static_cast<int>(sqrt(heightWidthRatio * numPerChannel) + 0.5f);
//  auto nWidth = numPerChannel / nHeight;
//  auto haarOffsetW = width/ (nWidth + 1.f);
//  auto haarOffsetH = height/(nHeight + 1.f);
//  auto haarWidth = static_cast<int>(haarOffsetW * 2.f);
//  auto haarHeight = static_cast<int>(haarOffsetH * 2.f);

// // cout << "num: " << num << " numPerChannel: " << numPerChannel << " nHeight: " << nHeight << " nWidht: " << nWidth;
////  cout << " haarWidth: " << haarWidth << " haarHeight: " << haarHeight << " haarOffsetW:" << haarOffsetW << " haarOffsetH:" << haarOffsetH << endl;

//  for( auto c = 0; c < ftrParams._numCh; ++c )
//  {
//      for( auto x = 0; x < nWidth; ++x )
//      {
//        for( auto y = 0; y < nHeight; ++y )
//        {
//          auto haarftr = vca::Icf::BoxSum();

//          haarftr.box.x = static_cast<int>( haarOffsetW * x + paddingPixelWidth );
//          haarftr.box.y = static_cast<int>( haarOffsetH * y + paddingPixelHeight );
//          haarftr.box.w = haarWidth;
//          haarftr.box.h = haarHeight;
//          haarftr.box.channel = vca::Icf::LUV_L;
//          result.push_back( move( haarftr ));

//        }
//      }
//   }
//}





void ClfMilBoost::generateFtr(HaarFtrParams& ftrParams, vector<int>& num, vca::icf::FeaturepipeType& fptype, vca::icf::FeatureContainer& _featCon)
{
  vca::icf::BoxFeatureFactory::BoxFeatureConfig boxconf;


  if (fptype.quantdirtype_ == vca::icf::FeaturepipeType::noQuant)
  {
    boxconf.useQuanDir = false;
    boxconf.useDxDyMag = true;
    boxconf.useAbsDxDyMag = true;
  }
  if (fptype.colortype_ == vca::icf::FeaturepipeType::gray)
  {
    boxconf.useLuv = false;
    boxconf.useGray = true;
  }
  //auto paddingFactor = 0.25f;
  auto paddingFactor = 0.25f;
  vca::icf::BoxFeatureFactory bfact(boxconf);

  for (int i = 0; i < num.size(); ++i)
    bfact.generateFeaturesGrid(_featCon, num[i], ftrParams._width, ftrParams._height, ftrParams._paddingFactor, 0.5f);

}

void ClfMilBoost::classify(vector< Sample>& samples, vector<cv::Mat>& sampleFeatureValues)
{
  int numsamples = samples.size();
  // prob.clear();
  // prob.resize( numsamples, 0.f );
  std::vector<float> tr;

  //if (!sampleFeatureValues.ftrsComputed())
  //  computeFtr(TODO, sampleFeatureValues, _ftrs);  

  //for (auto j = 0; j < numsamples; j++)
  //{
  //  samples[j]._prob = 0;
  //}

  vector<ClfOnlineStump::stumpResult> stumpSum(samples.size(), { 0.0001f, 0.0001f });

  for (auto w = 0; w < _selectors.size(); w++)
  {
#pragma omp parallel for
    for (auto j = 0; j < numsamples; j++)
    {
      stumpSum[j] += _weakclf[_selectors[w]].classifyF(sampleFeatureValues, j);
    }
  }

//#pragma omp parallel for
  for (auto j = 0; j < numsamples; j++)
  {
    cout << "stumpSum " << stumpSum[j].pos << "  " << stumpSum[j].neg << endl;
 //   double tmp = 1. / (1. + exp(-samples[j]._prob));
 //   samples[j]._prob = 1.f / (1. + exp((-stumpSum[j].pos + stumpSum[j].neg) / _selectors.size()));
    float p = (stumpSum[j].pos - stumpSum[j].neg) / _selectors.size();
    samples[j]._prob = 0.5f + 0.5f* p / (1. + abs( p));
    //double tmp = samples[j]._prob / (1. + abs(samples[j]._prob));
   // cout << tmp << " " << exp(-samples[j]._prob) << " " << samples[j]._prob << endl;
    //float absMin = abs(stumpSum[j].pos)< abs(stumpSum[j].neg) ? stumpSum[j].pos : stumpSum[j].neg;
    //samples[j]._prob = exp(stumpSum[j].pos - absMin) / (exp(stumpSum[j].pos - absMin) + exp(stumpSum[j].neg - absMin));
  }
}



//void ClfMilBoost::updateFtrPointer( map<vca::Icf::ChannelType,cv::Mat>&  integralImgs )
//{
//  for( auto i = 0; i < _ftrs.size(); ++i )
//  {
//    _ftrs[ i ].set(integralImgs,0,0);
//  }
//}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
void ClfMilBoost::init(ClfMilBoostParams& params, HaarFtrParams& ftrParams, vca::icf::FeaturepipeType& fptype)
{
  _nSelector = params._numSel;

  generateFtr(ftrParams, params._numFeat, fptype, _featCon);
  int numFeatAll = 0;
  for (int i = 0; i < params._numFeat.size(); ++i)
    numFeatAll += params._numFeat[i];

  auto nFeat = _featCon.numBoxes();
  _nSelector = nFeat *  params._numSel / numFeatAll;
  sassert(_nSelector <= nFeat);
  //  sassert(_nSelector <= params._numSel);
  _weakclf.clear();
  _weakclf.resize(nFeat);

  for (int k = 0; k < _weakclf.size(); k++)
  {
    _weakclf[k] = ClfOnlineStump(k);
    //_weakclf[k]._ftr = &_ftrs[k];
    _weakclf[k]._lRate = params._lRate;
  }
  //  _selectors.resize( min(params._numSel, static_cast<int>( _ftrs.size())) );

  _pospred.resize(_weakclf.size());
  _negpred.resize(_weakclf.size());
  _poslikl.resize(_weakclf.size());
  _neglikl.resize(_weakclf.size());
  _liklv.resize(_weakclf.size());
  _order.resize(_weakclf.size());
  std::iota(_order.begin(), _order.end(), 0);
  //_selectors = _order;
  // std::iota(_selectors.begin(), _selectors.end(), 0);
}

void ClfMilBoost::genNewFtr(ClfMilBoostParams& params, HaarFtrParams& ftrParams, vca::icf::FeaturepipeType& fptype)
{
  generateFtr(ftrParams, params._numFeat, fptype, _featCon);
}


void ClfMilBoost::update(vector<cv::Mat>&posx, vector<cv::Mat>&negx)//, string folder, int frameNum)
{
  int numneg = negx[0].cols;
  int numpos = posx[0].cols;

  // compute ftrs
  //if (!posx.ftrsComputed())
  //  computeFtr(TODO, posx, _ftrs);
  //if (!negx.ftrsComputed())
  //  computeFtr(TODO, negx, _ftrs);

  _selectors.clear();

  // initialize H
  vector<float> Hpos(numpos, 0.0f);
  vector<float> Hneg(numneg, 0.0f);

  vector<float> posw(numpos);
  vector<float> negw(numneg);
  float eps = 1e-10f;

  // train all weak classifiers without weights
#pragma omp parallel for
  for (int m = 0; m < _weakclf.size(); m++)
  {
    _weakclf[m].update(posx, negx);
    _weakclf[m].classifySetF(posx, _pospred[m]);
    _weakclf[m].classifySetF(negx, _negpred[m]);
  }

  // pick the best features
  for (int s = 0; s < _nSelector; s++)
  {
    // compute errors/likl for all weak clfs
#pragma omp parallel for
    for (int w = 0; w < (int)_weakclf.size(); w++)
    {
      double lll = 1.0f;
      for (int j = 0; j < numpos; j++)
      {
        //      lll *= (1. - sigmoid(Hpos[j] + _pospred[w][j]));
        lll = min(lll, (1. - sigmoid(Hpos[j] + _pospred[w][j].pos - _pospred[w][j].neg)));
      }
      _poslikl[w] = static_cast<float> (std::log(1.f - lll + eps));
      //   _poslikl[w] = 1.f;

      lll = 0.0f;
      for (int j = 0; j < numneg; j++)
        lll += static_cast<double> (std::log(eps + 1. - sigmoid(Hneg[j] + _negpred[w][j].pos - _negpred[w][j].neg)));
      _neglikl[w] = static_cast<float>(lll);

      //if( lll  == numeric_limits<double>::infinity( ))
      //{
      //  cout << "!!" << lll << endl;
      //  for (int j = 0; j < numneg; j++)
      //  {
      //    if( static_cast<double> (std::log(eps + 1. - sigmoid(Hneg[j] + _negpred[w][j]))) == numeric_limits<double>::infinity( ) )
      //     cout << Hneg[j]  << " " << _negpred[w][j]<< " " << sigmoid(Hneg[j] + _negpred[w][j]) << " "<< std::log(eps + 1.f - sigmoid(Hneg[j] + _negpred[w][j])) <<endl;

      //  }

      //  cout << "!!" << endl;
      //}
      //    _liklv[w] = _poslikl[w] + _neglikl[w] / numneg;
      _liklv[w] = 1.f;
    }

    // pick best weak clf			
    std::sort(_order.begin(), _order.end(), [&](int i, int j){  return _liklv[i] < _liklv[j]; });
    ////for (auto k = 0; k < _order.size(); k++)
    //for (auto k = 0; k < 10; k++)
    //{
    //  cout << s << " " <<  k << " " <<  _order[k] << " " <<_liklv[ _order[k] ] << " " <<  _poslikl[_order[k]]  << " " << _neglikl[ _order[ k ] ] << endl;
    //}

    // find best weakclf that isn't already included
    for (auto k = 0; k < _order.size(); k++)
    {
      if (count(_selectors.begin(), _selectors.end(), _order[k]) == 0)
      {
        _selectors.push_back(_order[k]);
        break;
      }
    }
    // update H = H + h_m

    //#pragma omp parallel for
    for (int k = 0; k < numpos; k++)
    {
      Hpos[k] += _pospred[_selectors[s]][k].pos - _pospred[_selectors[s]][k].neg;
    }
    //#pragma omp parallel for
    for (int k = 0; k < numneg; k++)
    {
      Hneg[k] += _negpred[_selectors[s]][k].pos - _negpred[_selectors[s]][k].neg;
    }
  }
}

void ClfMilBoost::drawFeatures(cv::Mat& img, float scale)//, string folder, int frameNumber)
{
  for (auto i = 0; i < _featCon.numBoxes(); ++i)
  {
    int x = _featCon.getBox(i)->box.x;
    int y = _featCon.getBox(i)->box.y;
    int w = _featCon.getBox(i)->box.w;
    int h = _featCon.getBox(i)->box.h;
    cv::rectangle(img, cv::Rect(static_cast<int>(scale*x), static_cast<int>(scale*y),
      static_cast<int>(scale*w), static_cast<int>(scale*h)), cv::Scalar(255, 255, 0), 1);
  }

}

void ClfMilBoost::preselect(cv::Mat& segmentMask, float scale)
{
  cv::Mat result;
  segmentMask.copyTo(result);
  for (auto i = 0; i < _featCon.numBoxes(); ++i)
  {
    int x = _featCon.getBox(i)->box.x;
    int y = _featCon.getBox(i)->box.y;
    int w = _featCon.getBox(i)->box.w;
    int h = _featCon.getBox(i)->box.h;
    cv::Rect roi(static_cast<int>(scale*x), static_cast<int>(scale*y),
      static_cast<int>(scale*w), static_cast<int>(scale*h));
    double s = cv::sum(segmentMask(roi))[0]/(255*scale*scale*w*h);
    if (s>0.2)
    {
      cv::rectangle(result, roi, cv::Scalar(255, 0, 0), 1);
      cout << s << " ";
    }
    //cv::rectangle(result, roi, cv::Scalar(255, 0, 0), 2);
  }
  cv::imshow("s", result);
}

void ClfMilBoost::logAll(string path, cv::Mat img, float scale)
{
  ofstream log1File(path + "posMuSigSq.txt", ios::app);
  ofstream log0File(path + "negMuSigSq.txt", ios::app);
  for (int m = 0; m < _weakclf.size(); m++)
  {
    auto mu0 = _weakclf[m]._mu0;
    auto mu1 = _weakclf[m]._mu1;
    auto sigSq0 = _weakclf[m]._sigSq0;
    auto sigSq1 = _weakclf[m]._sigSq1;
    if (log1File.is_open())
    {
      log1File << mu1 << " " << sigSq1 << " \n";      
    }
    else cout << "Unable to open posLogFile";
    if (log0File.is_open())
    {
      log0File << mu0 << " " << sigSq0 << " \n";
    }
    else cout << "Unable to open posLogFile";
  }
  log1File.close();
  log0File.close();


  cv::Mat imgCopy;
  img.copyTo(imgCopy);
  ofstream sampPosLogFile(path + "samplePositions.txt", ios::app);
  for (auto i = 0; i < _featCon.numBoxes(); ++i)
  {
    int x = _featCon.getBox(i)->box.x;
    int y = _featCon.getBox(i)->box.y;
    int w = _featCon.getBox(i)->box.w;
    int h = _featCon.getBox(i)->box.h;


    cv::Rect featBox;
    featBox = cv::Rect(static_cast<int>(scale*x), static_cast<int>(scale*y),
        static_cast<int>(scale*w), static_cast<int>(scale*h));
    //cv::Mat roi = imgCopy(featBox);
    //cv::imwrite(path + "//feat" + static_cast<ostringstream*>(&(ostringstream() << i+1))->str() + ".png", roi);

    sampPosLogFile << static_cast<int>(scale*x) << " " << static_cast<int>(scale*y) << " " << static_cast<int>(scale*w) << " " << static_cast<int>(scale*h) << "\n";
  }
  sampPosLogFile.close();
}


