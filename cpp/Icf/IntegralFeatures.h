#pragma once

#include <opencv2/core/core.hpp>
//#include "utils/RndNumbers.h"
#include "FeaturePipe.h"
#include "features/FeaturesLogger.h"
#include "features/FeaturesMacros.h"
#include "IntegralFeaturesObserver.h"



namespace vca
{
  namespace icf
  {
    
    class VCA_API IntegralFeatures
    {
    public:

      IntegralFeatures( shared_ptr<Featurepipe> ft );

      void update();

      void update( const cv::Mat& img );

      void registerObserver( IntegralFeaturesObserver* observer );

      void deregisterObserver( IntegralFeaturesObserver* observer );

      inline int size(){ return pipe->size(); }

      inline int height(){ return pipe->height(); }

      inline int width(){ return pipe->width(); }

      inline int border(){ return pipe->border(); }

      inline int getOffset( int x, int y ){ return y * (width() + 1 ) + x; }

      inline std::pair<int,int> getPoint( int offset ){ return std::pair<int,int>( offset% (width() +1), offset/ (width() + 1 )); }

      inline void setBorder( int border ){ pipe->setBorder( border ); }

      inline FeaturepipeType getFeaturePipeType(){ return pipe->getFeaturePipeType(); }

      void getFeatureImg(  vca::icf::ChannelType channel, cv::Mat& img ) const;

      void getFeatureImgs( vector< const cv::Mat>& imgs ) const;

      void activateChannel( ChannelType channel );

      void activateAllChannels();

      int nChannels()
      {
        sassert(activeChannelTypes.size() == intImgs_.size());
        return intImgs_.size();
      }

      vca::icf::ChannelType getChannelType(int idx){ return activeChannelTypes[idx]; }

      vector< vca::icf::ChannelType> activeChannelTypes;

      map< vca::icf::ChannelType, cv::Mat > intImgs_;
      ///< list with integral images

      vector< IntegralFeaturesObserver* > registeredObserver;

      void updateFeatureChannelPointer();      

      shared_ptr<Featurepipe> pipe;

      const cv::Mat& getInputImg(){ return pipe->getInputImg();}

     
    };
  }
}
