#pragma once

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
#include "features/Icf/ParDer1.h"
#include "features/Icf/LuvMaps.h"
#include "features/Icf/FeaturePipe.h"
#include "features/Icf/GrayChannel.h"
#include "videostreams/Converter/ToRgb.h"
#include "features/FeaturesLogger.h"
#include "features/FeaturesMacros.h"
#include <chrono>
#include "convTri.h"

namespace vca
{

  namespace icf
  {
    template< class QuantDirDerType >
    class VCA_API FeaturePipeGradients1Gray : public Featurepipe
    {
    public:

      /**
      *
      */

      FeaturePipeGradients1Gray(const FeaturepipeType& t)
      {
        quantDir_.cfg(t);
        height_ = width_ = 0;
        border_ = 0;

        featurepipeType_ = t;

        sassert(featurepipeType_.colortype_ == FeaturepipeType::gray || featurepipeType_.colortype_ == FeaturepipeType::non);
        sassert(featurepipeType_.dxdytype_ == FeaturepipeType::nonabs1);
        sassert(featurepipeType_.quantdirtype_ == quantDir_.getType());
        //// featurepipeType_.partDerType = maxParDer;
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

      void calcMap(const cv::Mat& src) override
      {
        //auto t0 = std::chrono::high_resolution_clock::now();

        //inputImg = src;
        Vca::Conv::ToGray<>(src, inputImg);

        //auto t1 = std::chrono::high_resolution_clock::now();
        //cout << "      tov rgb: " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << endl;

        //cout << img.rows << " " << img.cols << " " << border_ << endl;
        if (border_ > 0)
          cv::copyMakeBorder(inputImg, inputImg, border_, border_, border_, border_, cv::BORDER_REFLECT);
        //auto t2 = std::chrono::high_resolution_clock::now();
        //cout << "      copy make border: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << endl;

        height_ = inputImg.rows;
        width_ = inputImg.cols;




        cv::Mat inputImgFloat;
        inputImg.convertTo(inputImgFloat, CV_32FC1, 1. / 255.);
        graychannel.calcMap(inputImg);


        cv::Mat s0(inputImgFloat.size(), CV_32FC1);
        convTri::convTri1(inputImgFloat.ptr<float>(), s0.ptr<float>(), inputImgFloat.cols, inputImgFloat.rows, 1.f);

    
        par_.calcMapFloat(s0);

        quantDir_.calcMap(par_.dx, par_.dy);


 
      }


    private:

      GrayChannel graychannel;

      ParDer1 par_;

      QuantDirDerType quantDir_;
      ///< quantized gradient directions


    };

  }

}


