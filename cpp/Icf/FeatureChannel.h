
#pragma once
#include "opencv2/core/core.hpp"
#include "features/FeaturesLogger.h"
#include "features/FeaturesMacros.h"

namespace vca
{

  namespace icf
  {
    enum ChannelType{ LUV_L, LUV_U, LUV_V, GRAY, DIR_0, DIR_1, DIR_2, DIR_3, DIR_4, DIR_5, MAG, D_X, D_Y, D_XP, D_XN, D_YP, D_YN, D_Xabs, D_Yabs, D_XYabs, D_YXabs  };
 
    class VCA_API FeatureChannel
    {
    public:
                  
      virtual int nChannels( void ) const  = 0;
      
      virtual float getScalingFactor( ChannelType channel) =0;

      virtual void get( ChannelType channel, cv::Mat& img ) = 0;
      //{
      //  sassert( channelType2FeatureChannel.count(channel)>0);
      //  img = channelType2FeatureChannel[channel];
      //}

      protected:

   //   map<ChannelType, cv::Mat> channelType2FeatureChannel;
    };

  }

}
