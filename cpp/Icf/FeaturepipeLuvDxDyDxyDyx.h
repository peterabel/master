#pragma once

#pragma once

#include "features/Icf/QuantDirDer.h"
#include "features/Icf/MaxParDer.h"
#include "features/Icf/LuvMaps.h"
#include "features/Icf/FeaturePipe.h"
#include "videostreams/Converter/ToRgb.h"
#include "features/FeaturesLogger.h"
#include "features/FeaturesMacros.h"
#include "MaxParDerAbs.h"
#include "Magnitude.h"
#include "MaxParDer4.h"

namespace vca
{

  namespace icf
  {
    class VCA_API FeaturepipeLuvDxDyDxyDyx : public Featurepipe
    {
    public:

      /**
      *
      */

      FeaturepipeLuvDxDyDxyDyx(const FeaturepipeType& t)
      {

        quantDir_.cfg(t);

        height_ = width_ = 0;
        border_ = 0;

        featurepipeType_ = t;
        sassert(featurepipeType_.dxdytype_ == FeaturepipeType::abs4);
        sassert(featurepipeType_.colortype_ == FeaturepipeType::luv);
        sassert(featurepipeType_.quantdirtype_ == FeaturepipeType::noQuant);
       //// featurepipeType_.partDerType = maxParDer;

        channelType2FeatureChannel[LUV_L] = &luv_;
        channelType2FeatureChannel[LUV_U] = &luv_;
        channelType2FeatureChannel[LUV_V] = &luv_;


        channelType2FeatureChannel[MAG] = &quantDir_;

        channelType2FeatureChannel[D_Xabs] = &maxParAbs_;
        channelType2FeatureChannel[D_Yabs] = &maxParAbs_;   
        channelType2FeatureChannel[D_XYabs] = &maxParAbs_;
        channelType2FeatureChannel[D_YXabs] = &maxParAbs_;

      }

      void calcMap( const cv::Mat& src ) override
      {
      //  inputImg = src;
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
            quantDir_.calcMap( maxParAbs_.dx,  maxParAbs_.dy);
          }

#pragma omp section
          {
            luv_.calcMap( inputImg );    
          }
        }    
      }   


    private:


      MaxParDer4 maxParAbs_;
      ///< maximum partial derivatives

      Magnitude quantDir_;
      ///< quantized gradient directions

      LuvMaps luv_;
      ///< color maps


    };

  }

}






