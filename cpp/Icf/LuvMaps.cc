#include "features/Icf/LuvMaps.h"
#include "utils/Types/ArrUtils.h"

namespace vca
{

  const float icf::LuvMaps::un = 0.19793943f;

  const float icf::LuvMaps::vn = 0.46831096f;
  const float icf::LuvMaps::ythresh = 216.f/24389.f;

  //const float icf::LuvMaps::xr = 0.412453f / 255.0f;
  //const float icf::LuvMaps::xg = 0.357580f / 255.0f;
  //const float icf::LuvMaps::xb = 0.180423f / 255.0f;

  //const float icf::LuvMaps::yr = 0.212671f / 255.0f;
  //const float icf::LuvMaps::yg = 0.715160f / 255.0f;
  //const float icf::LuvMaps::yb = 0.072169f / 255.0f;

  //const float icf::LuvMaps::zr = 0.019334f / 255.0f;
  //const float icf::LuvMaps::zg = 0.119193f / 255.0f;
  //const float icf::LuvMaps::zb = 0.950227f / 255.0f;

  const float icf::LuvMaps::xr = 0.4123955889674142161f;
  const float icf::LuvMaps::xg = 0.3575834307637148171f;
  const float icf::LuvMaps::xb = 0.1804926473817015735f;

  const float icf::LuvMaps::yr = 0.2125862307855955516f;
  const float icf::LuvMaps::yg = 0.7151703037034108499f;
  const float icf::LuvMaps::yb = 0.07220049864333622685f;

  const float icf::LuvMaps::zr = 0.01929721549174694484f;
  const float icf::LuvMaps::zg = 0.1191838645808485318f;
  const float icf::LuvMaps::zb = 0.9504971251315797660f;

  const float icf::LuvMaps::fac = 24389.f / 27.f;

  const float  icf::LuvMaps::norm = 1.f / 354.f;
  const float  icf::LuvMaps::minu = -134 * icf::LuvMaps::norm;
  const float  icf::LuvMaps::minv = -140 * icf::LuvMaps::norm;

  //from http://www.mathworks.com/matlabcentral/fileexchange/28790-colorspace-transformations
  //Colorspace Transformations
  //by Pascal Getreuer

  #define WHITEPOINT_X	0.950456f
  #define WHITEPOINT_Y	1.0f
  #define WHITEPOINT_Z	1.088754f

  
  #define WHITEPOINT_U	((4*WHITEPOINT_X) \
    / (WHITEPOINT_X + 15 * WHITEPOINT_Y + 3 * WHITEPOINT_Z))
  #define WHITEPOINT_V	((9*WHITEPOINT_Y) \
    / (WHITEPOINT_X + 15 * WHITEPOINT_Y + 3 * WHITEPOINT_Z))
  #define INVGAMMACORRECTION(t)	\
    (((t) <= 0.0404482362771076) ? \
    ((t) / 12.92) : pow(((t)+0.055) / 1.055, 2.4))
#define LABF(t)	\
  ((t >= 8.85645167903563082e-3f) ? \
  pow(t, 0.333333333333333f) : (841.0f / 108.0f)*(t)+(4.0f / 29.0f))

  //

  /**
  *
  */

  icf::LuvMaps::LuvMaps()
  {
    isInit = false;
    init();
    //channelType2FeatureChannel[LUV_L]=lMap;
    //channelType2FeatureChannel[LUV_U]=uMap;
    //channelType2FeatureChannel[LUV_V]=vMap;
  }

  void icf::LuvMaps::init()
  {
    invgamma_lookupTable = vector<float>(256, 0.f);
    l_lookupTable = vector<float>(1025, 0.f);
    for (int i = 0; i < 1025; ++i)
    {
      float  y = (i / 1024.f);
      //double l = LABF(y);
      float l = 0.f;//LABF(y);
      if (y > ythresh)
        l = 116.f * powf(y, 1.f / 3.f) - 16.f;
      else
        l = y * fac;
      l_lookupTable[i] = l*norm;
    }
    for (int i = 0; i < 256; ++i)
    {
      double in = i / 255.;
      invgamma_lookupTable[i] = static_cast<float>(INVGAMMACORRECTION(in));
    }
    isInit = true;

  }

  /**
  *
  */

  void icf::LuvMaps::calcMap( const cv::Mat& src )
  {
    sassert( src.isContinuous() && src.type() == CV_8UC3 );
    sassert(isInit);


/*
    cv::Mat floatSrc;
    src.convertTo(floatSrc, CV_32F, 1./255);
    cv::Mat luvImg;
    cv::cvtColor(floatSrc, luvImg, CV_BGR2Luv);
    //L = L / 270, u = (u + 88) / 270, and
    //  % v = (v + 134) / 270.
    //cv::cvtColor(src, luvImg, CV_BGR2Luv);
    //luvImg.convertTo(luvImg, CV_32F);
    vector<cv::Mat> splitedLuv;
    cv::split(luvImg, splitedLuv);

    lMap = splitedLuv.at(0);
    uMap = splitedLuv.at(1);
    vMap = splitedLuv.at(2);

    //double minVal, maxVal;
    //cv::minMaxIdx(lMap, &minVal, &maxVal);
    //cout << "L: " << minVal << ", " << maxVal << endl;
    //cv::minMaxIdx(uMap, &minVal, &maxVal);
    //cout << "u: " << minVal << ", " << maxVal << endl;
    //cv::minMaxIdx(vMap, &minVal, &maxVal);
    //cout << "v: " << minVal << ", " << maxVal << endl;

    //lMap = lMap / 270;
    //uMap = (uMap + 88) / 270;
    //vMap = (vMap + 134) / 270;

    lMap = lMap / 354;
    uMap = (uMap + 134) / 354;
    vMap = (vMap + 140) / 354;

   //// double minVal, maxVal;
   // cv::minMaxIdx(lMap, &minVal, &maxVal);
   // cout << "L: " << minVal << ", " << maxVal << endl;
   // cv::minMaxIdx(uMap, &minVal, &maxVal);
   // cout << "u: " << minVal << ", " << maxVal << endl;
   // cv::minMaxIdx(vMap, &minVal, &maxVal);
   // cout << "v: " << minVal << ", " << maxVal << endl;

    sassert(src.size() == lMap.size())
    sassert( lMap.type() ==  CV_32FC1);
    sassert(src.size() == uMap.size())
    sassert(uMap.type() == CV_32FC1);
    sassert(src.size() == vMap.size())
    sassert(vMap.type() == CV_32FC1);*/



    Vca::match( src, lMap, CV_32FC1 );
    Vca::match( src, uMap, CV_32FC1 );
    Vca::match( src, vMap, CV_32FC1 );



    float *lMapPtr = lMap.ptr<float>();
    float *uMapPtr = uMap.ptr<float>();
    float *vMapPtr = vMap.ptr<float>();
    const uchar *srcPtr = src.ptr<uchar>();


    #pragma omp parallel for
    for( int i=0; i < src.rows * src.cols; ++i )
    {
      int rgbidx = 3 * i;
        if( srcPtr[ rgbidx + 2 ] == 0
          && srcPtr[ rgbidx + 1 ] == 0
          && srcPtr[ rgbidx ] == 0 )
        {
          lMapPtr[ i] = 0.f;
          uMapPtr[ i] = 0.f;
          vMapPtr[ i] = 0.f;
        }
        else
        {
          auto r = (srcPtr[rgbidx + 2] / 255.f);
          auto g = (srcPtr[rgbidx + 1] / 255.f);
          auto b = (srcPtr[rgbidx] / 255.f);
          //auto r = INVGAMMACORRECTION(srcPtr[rgbidx + 2]/255.f);
          //auto g = INVGAMMACORRECTION(srcPtr[rgbidx + 1] / 255.f);
          //auto b = INVGAMMACORRECTION(srcPtr[rgbidx] / 255.f);
      
          //auto r = invgamma_lookupTable[srcPtr[rgbidx + 2]];
          //auto g = invgamma_lookupTable[srcPtr[rgbidx + 1]];
          //auto b = invgamma_lookupTable[srcPtr[rgbidx]];

          auto x = r * xr + g* xg + b * xb;
          auto y = r * yr + g * yg + b * yb;
          auto z = r * zr + g * zg + b * zb;
 /*         float denom = x + 15 * y + 3 * z;
          float u1 = 0;
          float v1 = 0;
          if (denom > 0)
          {
             u1 = (4 * x) / denom;
             v1 = (9 * y) / denom;
          }*/
/*
         // y /= WHITEPOINT_Y;
          y = LABF(y);

          lMapPtr[i] = 116 * y - 16;
          uMapPtr[i] = 13 * (lMapPtr[i])*(u1 - WHITEPOINT_U);
          vMapPtr[i] = 13 * (lMapPtr[i])*(v1 - WHITEPOINT_V);*/

          //if( y > ythresh )
          //{
          //  lMapPtr[ i] = 116.f * powf(y, 1.f/3.f) - 16.f;
          //} else {
          //  lMapPtr[ i] = y * fac;
          //}
          lMapPtr[i] = l_lookupTable[static_cast<int>(y * 1024 + 0.5)];
          z = 1.f / (x + 15.f * y + 3.f * z);
          uMapPtr[ i] = 13.f * lMapPtr[ i] * (4.f * x*z -un)-minu;
          vMapPtr[ i] = 13.f * lMapPtr[ i] * (9.f * y*z - vn)-minv;


          //lMapPtr[i] = lMapPtr[i] / 354;
          //uMapPtr[i] = (uMapPtr[i] + 134) / 354;
          //vMapPtr[i] = (vMapPtr[i] + 140) / 354;

          //lMapPtr[i] = lMapPtr[i] / 270;
          //uMapPtr[i] = (uMapPtr[i] + 88) / 270;
          //vMapPtr[i] = (vMapPtr[i] + 134) / 270;


        }
    }


     //double minVal, maxVal;
     //cv::minMaxIdx(lMap, &minVal, &maxVal);
     //cout << "L: " << minVal << ", " << maxVal << endl;
     //cv::minMaxIdx(uMap, &minVal, &maxVal);
     //cout << "u: " << minVal << ", " << maxVal << endl;
     //cv::minMaxIdx(vMap, &minVal, &maxVal);
     //cout << "v: " << minVal << ", " << maxVal << endl;
  }
}
