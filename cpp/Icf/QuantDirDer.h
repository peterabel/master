/**
*
* Author        : --
*
* Module        : Features
* Classes       : QuantDirDer
* Namespace     : Vca::Icf
*
* Snippets      : --
* Resources     : --
*
* Description   : Calculate quantized directional derivatives
*
* Issues        : --
*
*/

#pragma once

#include <opencv2/core/core.hpp>
#include "cvgui/VsLogger/DspImage.h"
#include "FeatureChannel.h"
#include "features/FeaturesLogger.h"
#include "features/FeaturesMacros.h"
#include "FeaturePipeType.h"

namespace vca
{
  namespace icf
  {
    class VCA_API QuantDirDer : public FeatureChannel
    {
    public:


      /**
      * Default constructor
      * @param numQuants number of quantization steps
      */

      //inline QuantDirDer() : numQuants_( 6 ) { cfg( numQuants_ );}

      QuantDirDer(){};

      QuantDirDer(int numNorm, int numQuants = 6, bool softBin = false);

      /**
      * Configure
      */

      void cfg(const FeaturepipeType t){ cfg(t.nNormGradientSize, t.nGradientDir, t.softBin); }

      void cfg(int numNorm, int numQuants, bool softBin );

      /**
      *
      */



      void calcMap( const cv::Mat& dx, const cv::Mat& dy );
      void calcMap( const cv::Mat& dxp, const cv::Mat& dxn, const cv::Mat& dyp, const cv::Mat& dyn);

      virtual float getQuantValue( float ) = 0;

      virtual FeaturepipeType::quantDirType getType() = 0;


      int nChannels( void ) const override { return numQuants_ + 1;  };

      float getScalingFactor( ChannelType channel) override
      {
        sassert( channel == DIR_0 || channel == DIR_1 || channel == DIR_2 || channel == DIR_3 || channel == DIR_4 || channel == DIR_5 || channel == MAG ); 
        if (numNorm_ > -1)
          return 1.94f;
        return  1.6f;
      }

      void get( ChannelType channel, cv::Mat& img ) override
      {
        if( channel == DIR_0 )
          img = dirQuants[0];
        else   if( channel == DIR_1 )
          img = dirQuants[1];
        else   if( channel == DIR_2 )
          img = dirQuants[2];
        else   if( channel == DIR_3 )
          img = dirQuants[3];
        else   if( channel == DIR_4 )
          img = dirQuants[4];
        else   if( channel == DIR_5 )
          img = dirQuants[5];
        else   if( channel == MAG )
          img = mag;
      }


    private:

      void alloc( const cv::Mat& src );

      void GradientMagnitude( const cv::Mat& dx, const cv::Mat& dy );
      void GradientDirection( const cv::Mat& dx, const cv::Mat& dy, float eps );

      void GradientMagnitude( const cv::Mat& dxp, const cv::Mat& dxn, const cv::Mat& dyp, const cv::Mat& dyn );
      void GradientDirection( const cv::Mat& dxp, const cv::Mat& dxn, const cv::Mat& dyp, const cv::Mat& dyn, float eps );

    public:

      cv::Mat mag;
      ///< gradient magnitude

      vector< cv::Mat > dirQuants;
      ///< memory pool for quantized, directional gradient magnitudes

    private:

      int numNorm_ = 19;

      int numQuants_;
      ///< number of quantization steps

      float binWidth_;
      ///< bin width

      float eps_;

      vector< float* > mapPtr_;
      bool softbin_;
      ///< pointer to single feature maps

    };
    class VCA_API QuantDirDerMag : public QuantDirDer
    {
      //c++11 add final
      inline float getQuantValue( float magnitude ) final { return magnitude; } 
    public:
      inline FeaturepipeType::quantDirType  getType() { return FeaturepipeType::quantDirMag; }

    };
    class VCA_API QuantDirDerNorm : public QuantDirDer
    {
      inline float getQuantValue( float magnitude ) final { return 1.f; } 
    public:
      inline FeaturepipeType::quantDirType  getType() { return FeaturepipeType::quantDirNorm; }
    };
  }
}
