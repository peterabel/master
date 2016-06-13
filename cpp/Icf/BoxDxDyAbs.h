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
    class VCA_API BoxDxDyAbs  : public BoxFeature
    {
    public:
      inline BoxDxDyAbs();

      inline BoxDxDyAbs( int x_, int y_, int w_, int h_, ChannelType channel_, float direction_, bool useMag_, bool useAbs_ );

      ~BoxDxDyAbs() {}

      BoxType getFeatType() const override{ return absdxdyBox; } 

      /*write to stream*/
      friend VCA_API std::ostream& operator<<(std::ostream& out, const BoxDxDyAbs& box);

      /*read from stream*/
      friend VCA_API istream& operator>>( istream& in, BoxDxDyAbs& box );

      void activateChannels( shared_ptr<IntegralFeatures> feat ) const override;


      std::ostream& output(std::ostream& out) const override;

      std::istream& input( istream& in ) override;

      inline float eval( const uint off =0) const;

      inline bool set(  map< ChannelType, cv::Mat >& integralMap, int y, int x );

      scalingFactors scaleFeatBox(IntegralFeatures& integralFeat, BoxDxDyAbs& tar, float scaleFactor, bool forceRoundDown = false) const;


    public:

      bool useMag;
      float direction; // direction between 0 and 2pi
      bool useAbs;
      const double *upLeft_dxP_, *lowLeft_dxP_;
      const double *upLeft_dyP_, *lowLeft_dyP_;
      const double *upLeft_dxN_, *lowLeft_dxN_;
      const double *upLeft_dyN_, *lowLeft_dyN_;
    };
  }
}


