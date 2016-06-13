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
#include "features/Icf/LuvMaps.h"
#include "features/Icf/FeaturePipe.h"
#include "videostreams/Converter/ToRgb.h"
#include "features/FeaturesLogger.h"
#include "features/FeaturesMacros.h"
#include <chrono>

namespace vca
{

  namespace icf
  {
    template< class QuantDirDerType >
    class VCA_API FeaturePipeGradientsLuv : public Featurepipe
    {
    public:

      /**
      *
      */

      FeaturePipeGradientsLuv(const FeaturepipeType& t)
      {
        quantDir_.cfg(t);
        height_ = width_ = 0;
        border_ = 0;

        featurepipeType_ = t;

       sassert( featurepipeType_.colortype_ == FeaturepipeType::luv);
       sassert(featurepipeType_.dxdytype_ == FeaturepipeType::nonabs);
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

        channelType2FeatureChannel[D_X] = &maxPar_;
        channelType2FeatureChannel[D_Y] = &maxPar_;

      }

      void calcMap( const cv::Mat& src ) override
      {
		//auto t0 = std::chrono::high_resolution_clock::now();

       //inputImg = src;
        Vca::Conv::ToRgb(src,inputImg);
		//auto t1 = std::chrono::high_resolution_clock::now();
		//cout << "      tov rgb: " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << endl;

		//cout << img.rows << " " << img.cols << " " << border_ << endl;
        if( border_ > 0 )
          cv::copyMakeBorder(inputImg, inputImg, border_, border_, border_, border_, cv::BORDER_REFLECT);
		//auto t2 = std::chrono::high_resolution_clock::now();
		//cout << "      copy make border: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << endl;

		height_ = inputImg.rows;
        width_ = inputImg.cols;
        if( !inputImg.isContinuous() )
        {
          inputImg = inputImg.clone();
        }
		//auto t3 = std::chrono::high_resolution_clock::now();
		//cout << "      clone?: " << std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count() << endl;

#pragma omp parallel sections
        {
#pragma omp section
          {
			//auto t0_grad = std::chrono::high_resolution_clock::now();
            maxPar_.calcMap( inputImg );
			//auto t1_grad = std::chrono::high_resolution_clock::now();
			//cout << "      partDer: " << std::chrono::duration_cast<std::chrono::milliseconds>(t1_grad - t0_grad).count() << endl;
            quantDir_.calcMap( maxPar_.dx, maxPar_.dy );
			//auto t2_grad = std::chrono::high_resolution_clock::now();
			//cout << "      quant dir: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2_grad - t1_grad).count() << endl;

		  }

#pragma omp section
          {
			//auto t0_luv = std::chrono::high_resolution_clock::now();
            luv_.calcMap( inputImg );    
			//auto t1_luv = std::chrono::high_resolution_clock::now();
			//cout << "      luv: " << std::chrono::duration_cast<std::chrono::milliseconds>(t1_luv - t0_luv).count() << endl;

			}
        }    
      }   


    private:


      MaxParDer maxPar_;
      ///< maximum partial derivatives

      QuantDirDerType quantDir_;
      ///< quantized gradient directions

      LuvMaps luv_;
      ///< color maps


    };

  }

}
