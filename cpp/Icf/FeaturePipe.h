

#pragma once

#include "features/FeaturesLogger.h"
#include "features/FeaturesMacros.h"
#include "opencv2/core/core.hpp"
#include "FeaturePipeType.h"
#include "FeatureChannel.h"

namespace vca
{
  namespace icf
  { 

    class VCA_API Featurepipe
    {
    public:

      virtual void calcMap( const cv::Mat& src ) = 0;

      virtual int nMarginPixelNeeded()
      {
        if (featurepipeType_.quantdirtype_ == FeaturepipeType::noQuant)
          return 0;
        else
          return max(featurepipeType_.nNormGradientSize / 2, 1) + 1;
      }

      vector< ChannelType> getChannelTypes(){ vector<ChannelType> t; for (auto& c : channelType2FeatureChannel) t.push_back(c.first); return std::move(t); }

      FeaturepipeType getFeaturePipeType(){ return featurepipeType_;}

   
      virtual void get( ChannelType channel, cv::Mat& img )
      {
          sassert(channelType2FeatureChannel.count(channel)>0);
          channelType2FeatureChannel[channel]->get( channel, img);
      }


      virtual void get( vector<cv::Mat>& imgs)
      {
        for (auto& c : channelType2FeatureChannel)
        {
          cv::Mat img;
          c.second->get(c.first, img);
          imgs.emplace_back(img);
        }
      }


      virtual bool hasChannel( ChannelType channel )
      {
        return (channelType2FeatureChannel.count( channel )> 0 );
      }

      virtual inline int size() 
      {
        return featurepipeType_.nChannels();
      };

      virtual float getScalingFactor( ChannelType channel )
      {
        sassert(channelType2FeatureChannel.count(channel)>0);
        return channelType2FeatureChannel[channel]->getScalingFactor( channel);
      }


      inline int height(){ return height_;  }
      inline int width(){ return width_; }
      inline int border(){ return border_; }      
      inline void setBorder( int border ){ border_ = border; }


      const cv::Mat& getInputImg(){ return inputImg; }

    protected:
      cv::Mat inputImg;
      int height_, width_, border_;
      FeaturepipeType featurepipeType_;
      map<ChannelType,FeatureChannel*> channelType2FeatureChannel; 
    };
    
  }
}


