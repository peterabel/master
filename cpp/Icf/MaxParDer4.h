#pragma once

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

    class VCA_API MaxParDer4 : public FeatureChannel
    {
    public:

      /**
      *
      */

      MaxParDer4();

      /**
      *
      */

      //partDerType getType( ){ return maxParDer; }


      void calcMap( const cv::Mat& src );

      int nChannels( void ) const override { return 2; };

      float getScalingFactor( ChannelType channel) override
      {
        sassert( channel == D_Xabs || channel == D_Yabs || channel == D_XYabs || channel == D_YXabs ); 
        //TODO??? scaingfactor
        return 1.f;
      }


      void get( ChannelType channel, cv::Mat& img ) override
      {
        if( channel == D_Xabs )
          img = dx;
        else if( channel == D_Yabs )
          img = dy;
        else if( channel == D_YXabs )
          img = dyx;
        else if ( channel == D_XYabs )
          img = dxy;
      }

    public:

      cv::Mat dx,dy, dxy, dyx;
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


