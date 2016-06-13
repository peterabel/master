/**
*
* Author        : --
*
* Module        : Features
* Classes       : MaxParDer
* Namespace     : Vca::Icf
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


#include "features/Icf/FeatureChannel.h"
#include <opencv2/core/core.hpp>
#include "cvgui/VsLogger/DspImage.h"
#include "features/FeaturesLogger.h"
#include "features/FeaturesMacros.h"

namespace vca
{

  namespace icf
  {

    class VCA_API ParDer : public FeatureChannel
    {
    public:

      /**
      *
      */

      ParDer();

      /**
      *
      */

     // partDerType getType() { return partDer; };

     // virtual void calcMap( const cv::Mat& src );
      void calcMap( const cv::Mat& src );
      void calcMapFloat(const cv::Mat& src);


      /**
      *
      */


      int nChannels( void ) const override { return 2; };

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


    private:

      /**
      *
      */


    private:

    };

  }

}
