#pragma once
#include "FeatureChannel.h"
#include "ChannelBox.h"
#include "IntegralFeatures.h"

namespace vca
{
  namespace icf
  {

    struct scalingFactors
    {
      float featureScale;
      float scaleCorrection;
    };
    class VCA_API BoxFeature
    {
    public:
      enum BoxType{ sumBox, sumBoxQuantNorm, dxdyBox, absdxdyBox, quandxdyBox, dxdydxydyxBox};

      virtual ~BoxFeature() {} 
      virtual BoxType getFeatType() const = 0;

      virtual void activateChannels( shared_ptr<IntegralFeatures> feat ) const = 0;

      virtual std::ostream& output(std::ostream& out) const = 0;

      virtual std::istream& input( istream& in ) = 0;

      /*write to stream*/
      friend VCA_API std::ostream& operator<<(std::ostream& out, const BoxFeature& fbox){ return fbox.output( out );}

      /*read from stream*/
      friend VCA_API  std::istream& operator>>( istream& in, BoxFeature& fbox ){ return fbox.input( in );}

      ChannelBox box;

    };

  }
}

