#pragma once

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
    class VCA_API BoxSumQuantNorm : public BoxFeature
    {
    public:

      inline BoxSumQuantNorm();

      inline BoxSumQuantNorm(int x_, int y_, int w_, int h_, ChannelType channel_);

      ~BoxSumQuantNorm() {}

      BoxType getFeatType() const  override{ return sumBoxQuantNorm; }
      void activateChannels(shared_ptr<IntegralFeatures> feat) const override;

      /*write to stream*/
      friend VCA_API std::ostream& operator<<(std::ostream& out, const BoxSumQuantNorm& box);

      /*read from stream*/
      friend VCA_API  istream& operator>>(istream& in, BoxSumQuantNorm& box);

      std::ostream& output(std::ostream& out) const override;

      std::istream& input(istream& in) override;

      inline float eval(void) const;

      /**
      * Evaluate box filter
      * @param offset w.r.t. to iterator in x direction
      */
      inline float eval(const uint off) const;

      inline bool set(map< ChannelType, cv::Mat >& integralMap, int y, int x);

      scalingFactors scaleFeatBox(IntegralFeatures& integralFeat, BoxSumQuantNorm& tar, const float scaleFactor, bool forceRoundDown = false) const;

    public:
      
      const double *upLeft_, *lowLeft_;
      const double *upLeftNorm_, *lowLeftNorm_;


    };
  }
}



