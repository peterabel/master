/**
*
* Author        : --
*
* Module        : Features
* Classes       : FeaturePipe
* Namespace     : vca::Icf
*
* Snippets      : --
* Resources     : --
*
* Description   : Calculate single feature maps and return integral images
*
* Issues        : - Variable feature configuration missing
*
*/

#pragma once

#include "features/Icf/QuantDirDer.h"
#include "features/Icf/MaxParDer.h"
#include "features/Icf/FeaturePipe.h"
#include "videostreams/Converter/ToRgb.h"
#include "features/FeaturesLogger.h"
#include "features/FeaturesMacros.h"
#include "GrayChannel.h"
#include "ParDer.h"

namespace vca
{

  namespace icf
  {
    template< class QuantDirDerType >
    class VCA_API FeaturePipeGradient : public  Featurepipe
    {
    public:

      /**
      *
      */



      FeaturePipeGradient(const FeaturepipeType& t)
      {
        quantDir_.cfg(t);
        height_ = width_ = 0;
        border_ = 0;

        featurepipeType_ = t;
        sassert(featurepipeType_.dxdytype_ == FeaturepipeType::nonabs);
        sassert(featurepipeType_.colortype_ == FeaturepipeType::gray);
        sassert(featurepipeType_.quantdirtype_ == quantDir_.getType());
       // featurepipeType_.partDerType = partDer;

        channelType2FeatureChannel[GRAY] = &graychannel;

        channelType2FeatureChannel[DIR_0] = &quantDir_;
        channelType2FeatureChannel[DIR_1] = &quantDir_;
        channelType2FeatureChannel[DIR_2] = &quantDir_;
        channelType2FeatureChannel[DIR_3] = &quantDir_;
        channelType2FeatureChannel[DIR_4] = &quantDir_;
        channelType2FeatureChannel[DIR_5] = &quantDir_;

        channelType2FeatureChannel[MAG] = &quantDir_;

        channelType2FeatureChannel[D_X] = &par_;
        channelType2FeatureChannel[D_Y] = &par_;
      }


      void calcMap( const cv::Mat& src ) override
      {
        //inputImg = src;
        Vca::Conv::ToGray<>(src,inputImg);

        if( border_ > 0 )
          cv::copyMakeBorder( inputImg, inputImg, border_, border_, border_, border_, cv::BORDER_REFLECT );
        height_ = inputImg.rows;
        width_ = inputImg.cols;
        if( !inputImg.isContinuous() )
        {
          inputImg = inputImg.clone();
        }

        graychannel.calcMap( inputImg);
        par_.calcMap( graychannel.grayChannel );
        quantDir_.calcMap( par_.dx, par_.dy );
      }



    private:

      GrayChannel graychannel;

      ParDer par_;
      ///< maximum partial derivatives

      QuantDirDerType quantDir_;
      ///< quantized gradient directions


    };

  }

}
