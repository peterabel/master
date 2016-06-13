/**
*
* Author        : --
*
* Module        : Features
* Classes       : MaxParDer
* Namespace     : vca::Icf
*
* Snippets      : --
* Resources     : --
*
* Description   : --
*
* Issues        : --
*
*/

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

    class VCA_API MaxParDer : public FeatureChannel
    {
    public:

      /**
      *
      */

      MaxParDer();

      /**
      *
      */

      //partDerType getType( ){ return maxParDer; }


      void calcMap( const cv::Mat& src );

      void calcMap(const cv::Mat& src1, const cv::Mat& src2, const cv::Mat& src3);
      int nChannels(void) const override { return 2; };

      float getScalingFactor( ChannelType channel) override
      {
        sassert( channel == D_X || channel == D_Y ); 
        //TODO??? scaingfactor
        return 1.f;
      }


      void get( ChannelType channel, cv::Mat& img ) override
      {
        if( channel == D_X )
          img = dx;
        else if( channel == D_Y )
          img = dy;
      }

    public:

      cv::Mat dx,dy;
      ///< partial derivatives, CV_32S1

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
