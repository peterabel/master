

#pragma once

#include <opencv2/core/core.hpp>
#include "features/FeaturesLogger.h"
#include "features/FeaturesMacros.h"
#include "FeatureChannel.h"
#include "FeaturePipe.h"

namespace vca
{

  namespace icf
  {
    class VCA_API GrayChannel : public FeatureChannel
    {
    public:

      GrayChannel()
      {

      }

      void calcMap( const cv::Mat& src )
      {
       // sassert( src.type() == CV_8UC1);
        grayChannel = src;
      }

      int nChannels( void ) const override { return 1; };

      float getScalingFactor( ChannelType channel) override
      {
        sassert( channel == GRAY);
        return 2.f;
      }
      void get( ChannelType channel, cv::Mat& img ) override
      {
        if( channel == GRAY )
          img = grayChannel;
      }


    public:

      cv::Mat grayChannel;
 
    };
  }

}
