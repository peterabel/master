#pragma once

#include "features/FeaturesLogger.h"
#include "features/FeaturesMacros.h"
#include "FeaturePipe.h"
#include "IntegralFeatures.h"
#include "BoxFeature.h"

namespace vca
{
  namespace icf
  {
    class VCA_API BoxSum : public BoxFeature
    {
    public:

      inline BoxSum();

      inline BoxSum( int x_, int y_, int w_, int h_, ChannelType channel_ );

      ~BoxSum() {}

      BoxType getFeatType() const  override{ return sumBox; }
      void activateChannels( shared_ptr<IntegralFeatures> feat ) const override;

      /*write to stream*/
      friend VCA_API std::ostream& operator<<(std::ostream& out, const BoxSum& box);

      /*read from stream*/
      friend VCA_API  istream& operator>>( istream& in, BoxSum& box );

      std::ostream& output(std::ostream& out) const override;

      std::istream& input( istream& in ) override;

      inline float eval( void ) const;

      /**
      * Evaluate box filter
      * @param offset w.r.t. to iterator in x direction
      */
      inline float eval( const int off ) const;

      inline bool set( map< ChannelType, cv::Mat >& integralMap, int y, int x );

      scalingFactors scaleFeatBox(IntegralFeatures& integralFeat, BoxSum& tar, const float scaleFactor, bool forceRoundDown = false) const;

    public:


      const double *upLeft_, *lowLeft_;

    };
  }
}

