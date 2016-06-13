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
    class VCA_API BoxDxDy  : public BoxFeature
    {
    public:
      inline BoxDxDy();

      inline BoxDxDy( int x_, int y_, int w_, int h_, ChannelType channel_, float direction_, bool useMag_ );

      ~BoxDxDy() {}

       BoxType getFeatType() const override{ return dxdyBox; } 


       void activateChannels( shared_ptr<IntegralFeatures> feat ) const override;

      /*write to stream*/
      friend VCA_API std::ostream& operator<<(std::ostream& out, const BoxDxDy& box);

      /*read from stream*/
      friend VCA_API istream& operator>>( istream& in, BoxDxDy& box );

      std::ostream& output(std::ostream& out) const override;

      std::istream& input( istream& in ) override;


      inline float eval( const uint off = 0 ) const;

      inline bool set(  map< ChannelType, cv::Mat >& integralMap, int x, int y  );

      scalingFactors scaleFeatBox(IntegralFeatures& integralFeat, BoxDxDy& tar, const float scaleFactor, bool forceRoundDown = false) const;


    public:

      bool useMag;
      float direction; // direction between 0 and 2pi
      const double *upLeft_dx_, *lowLeft_dx_;
      const double *upLeft_dy_, *lowLeft_dy_;
    };


  }
}
