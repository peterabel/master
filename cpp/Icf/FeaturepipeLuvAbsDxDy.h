#pragma once

#include "features/Icf/QuantDirDer.h"
#include "features/Icf/MaxParDer.h"
#include "features/Icf/LuvMaps.h"
#include "features/Icf/FeaturePipe.h"
#include "videostreams/Converter/ToRgb.h"
#include "features/FeaturesLogger.h"
#include "features/FeaturesMacros.h"
#include "MaxParDerAbs.h"

namespace vca
{

  namespace icf
  {
    template< class QuantDirDerType >
    class VCA_API FeaturePipeGradientsLuvAbsDxDy : public Featurepipe
    {
    public:

      /**
      *
      */

      FeaturePipeGradientsLuvAbsDxDy(const FeaturepipeType& t)
      {

        quantDir_.cfg(t);

        height_ = width_ = 0;
        border_ = 0;

        featurepipeType_ = t;
        sassert(featurepipeType_.dxdytype_ == FeaturepipeType::abs);
        sassert(featurepipeType_.colortype_ == FeaturepipeType::luv);
        sassert(featurepipeType_.quantdirtype_ == quantDir_.getType());
       //// featurepipeType_.partDerType = maxParDer;

        channelType2FeatureChannel[LUV_L] = &luv_;
        channelType2FeatureChannel[LUV_U] = &luv_;
        channelType2FeatureChannel[LUV_V] = &luv_;

        channelType2FeatureChannel[DIR_0] = &quantDir_;
        channelType2FeatureChannel[DIR_1] = &quantDir_;
        channelType2FeatureChannel[DIR_2] = &quantDir_;
        channelType2FeatureChannel[DIR_3] = &quantDir_;
        channelType2FeatureChannel[DIR_4] = &quantDir_;
        channelType2FeatureChannel[DIR_5] = &quantDir_;

        channelType2FeatureChannel[MAG] = &quantDir_;

        channelType2FeatureChannel[D_XP] = &maxParAbs_;
        channelType2FeatureChannel[D_YP] = &maxParAbs_;   
        channelType2FeatureChannel[D_XN] = &maxParAbs_;
        channelType2FeatureChannel[D_YN] = &maxParAbs_;

      }

      void calcMap( const cv::Mat& src ) override
      {
       // inputImg = src;
        Vca::Conv::ToRgb(src,inputImg);
        //cout << img.rows << " " << img.cols << " " << border_ << endl;
        if( border_ > 0 )
          cv::copyMakeBorder(inputImg, inputImg, border_, border_, border_, border_, cv::BORDER_REFLECT);
        height_ = inputImg.rows;
        width_ = inputImg.cols;
        if( !inputImg.isContinuous() )
        {
          inputImg = inputImg.clone();
        }
#pragma omp parallel sections
        {
#pragma omp section
          {
            maxParAbs_.calcMap( inputImg );
            quantDir_.calcMap( maxParAbs_.dxp,  maxParAbs_.dxn, maxParAbs_.dyp, maxParAbs_.dyn);
          }

#pragma omp section
          {
            luv_.calcMap( inputImg );    
          }
        }    
      }   


    private:


      MaxParDerAbs maxParAbs_;
      ///< maximum partial derivatives

      QuantDirDerType quantDir_;
      ///< quantized gradient directions

      LuvMaps luv_;
      ///< color maps


    };

  }

}




