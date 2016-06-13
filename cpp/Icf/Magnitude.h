/**
*
* Author        : --
*
* Module        : Features
* Classes       : QuantDirDer
* Namespace     : vca::Icf
*
* Snippets      : --
* Resources     : --
*
* Description   : Calculate quantized directional derivatives
*
* Issues        : --
*
*/

#pragma once

#include <opencv2/core/core.hpp>
#include "cvgui/VsLogger/DspImage.h"
#include "FeatureChannel.h"
#include "features/FeaturesLogger.h"
#include "features/FeaturesMacros.h"
#include "FeaturePipeType.h"

namespace vca
{
  namespace icf
  {
    class VCA_API Magnitude : public FeatureChannel
    {
    public:

      inline Magnitude(){}

      void cfg(const FeaturepipeType t){};

      void calcMap( const cv::Mat& dx, const cv::Mat& dy );

      void calcMap(const cv::Mat& dxp, const cv::Mat& dxn, const cv::Mat& dyp, const cv::Mat& dyn);
      FeaturepipeType::quantDirType getType(){return FeaturepipeType::noQuant;}

      int nChannels( void ) const override { return  1;  };

      float getScalingFactor( ChannelType channel) override
      {
        sassert( channel == MAG ); 
        return  1.6f;
      }
      void get( ChannelType channel, cv::Mat& img ) override
      {
        if( channel == MAG )
          img = mag;
      }

    public:

      cv::Mat mag;
      ///< gradient magnitude


    };

  }
}
