#pragma once

#include "features/FeaturesLogger.h"
#include "features/FeaturesMacros.h"
#include "FeaturePipe.h"
#include "FeatureTypeStringConverter.h"

#include "IntegralFeatures.h"
#include "BoxFeature.h"


namespace vca
{
  namespace icf
  {
    class VCA_API BoxDxDyDxyDyx  : public BoxFeature
    {
    public:
      inline BoxDxDyDxyDyx();

      inline BoxDxDyDxyDyx( int x_, int y_, int w_, int h_, ChannelType channel_, float direction_, bool useMag_ );

      ~BoxDxDyDxyDyx() {}

      BoxType getFeatType() const override{ return dxdydxydyxBox; } 


      void activateChannels( shared_ptr<IntegralFeatures> feat ) const override;

      /*write to stream*/
      friend VCA_API std::ostream& operator<<(std::ostream& out, const BoxDxDyDxyDyx& box);

      /*read from stream*/
      friend VCA_API istream& operator>>( istream& in, BoxDxDyDxyDyx& box );

      std::ostream& output(std::ostream& out) const override;

      std::istream& input( istream& in ) override;


      inline float eval( const uint off = 0 ) const;

      inline bool set(  map< ChannelType, cv::Mat >& integralMap, int x, int y  );

      scalingFactors scaleFeatBox(IntegralFeatures& integralFeat, BoxDxDyDxyDyx& tar, const float scaleFactor, bool forceRoundDown = false) const;


    public:

      bool useMag;
      float direction; // direction between 0 and 2pi
      const double *upLeft_dx_, *lowLeft_dx_;
      const double *upLeft_dy_, *lowLeft_dy_;
      const double *upLeft_dxy_, *lowLeft_dxy_;
      const double *upLeft_dyx_, *lowLeft_dyx_;
    };


  }
}


