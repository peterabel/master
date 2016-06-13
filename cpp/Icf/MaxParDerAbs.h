
#pragma once

#include "FeatureChannel.h"
#include <opencv2/core/core.hpp>
#include "cvgui/VsLogger/DspImage.h"
#include "features/FeaturesLogger.h"
#include "features/FeaturesMacros.h"

namespace vca
{

  namespace icf
  {

    class VCA_API MaxParDerAbs : public FeatureChannel
    {
    public:

      MaxParDerAbs();

      void calcMap( const cv::Mat& src );

      int nChannels( void ) const override { return 4; };

      float getScalingFactor( ChannelType channel) override
      {
        sassert( channel == D_XP || channel == D_YP || channel == D_XN || channel == D_YN); 
        return 1.f;
      }


      void get( ChannelType channel, cv::Mat& img ) override
      {
        if( channel == D_XP )
          img = dxp;
        else if( channel == D_YP )
          img = dyp;
        else if( channel == D_XN )
          img = dxn;
        else if( channel == D_YN )
          img = dyn;
      }

    public:

      cv::Mat dxp,dyp,dxn,dyn;


    private:

      /**
      *
      */

      inline float maxAbsDiff( const float redDiff, const float blueDiff, const float greenDiff )
      {
        float result;
        if( abs( redDiff ) > abs( blueDiff ) )
          result = redDiff;
        else
          result = blueDiff;

        if( abs( greenDiff) > abs( result) )
          result = greenDiff;
        return result;
      }

    private:


    };

  }

}




