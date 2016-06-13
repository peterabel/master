/**
*
* Author        : --
*
* Module        : Features
* Classes       : LuvMaps
* Namespace     : vca::Icf
*
* Snippets      : --
* Resources     : --
*
* Description   : Calculate luv color maps
*
* Issues        : - Check (use) OpenCv implementation ??
*                 - Check normalisation of mix. matrix
*
*/

#pragma once

#include <opencv2/core/core.hpp>
#include "cvgui/VsLogger/DspImage.h"
#include "features/FeaturesLogger.h"
#include "features/FeaturesMacros.h"
#include "FeatureChannel.h"
#include "FeaturePipe.h"

namespace vca
{

  namespace icf
  {
    class VCA_API LuvMaps : public FeatureChannel
    {
    public:

      /**
      *
      */

      LuvMaps();

      /**
      *
      */

      void calcMap( const cv::Mat& src );

      int nChannels( void ) const override { return 3; };

      void get( ChannelType channel, cv::Mat& img ) override
      {
        if( channel == LUV_L )
          img = lMap;
        else if( channel == LUV_U )
          img = uMap;
        else if( channel == LUV_V )
          img = vMap;
      }

      void init();

      float getScalingFactor( ChannelType channel) override
      {
        sassert( channel == LUV_L || channel == LUV_U || channel == LUV_V );
        return 2.f;
      }


    public:

      cv::Mat lMap,uMap,vMap;
      ///< luv channels



    private:
      bool isInit;
      vector<float> l_lookupTable;
      vector<float> invgamma_lookupTable;

      static const float norm;
      static const float minu;
      static const float minv;

      static const float un;

      static const float vn;
      static const float ythresh;

      static const float xr;
      static const float xg;
      static const float xb;

      static const float yr;
      static const float yg;
      static const float yb;

      static const float zr;
      static const float zg;
      static const float zb;

      static const float fac;
    };
  }

}
