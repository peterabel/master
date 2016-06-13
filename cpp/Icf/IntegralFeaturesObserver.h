

#pragma once

#include <opencv2/core/core.hpp>
#include "features/FeaturesLogger.h"
#include "features/FeaturesMacros.h"

#include "FeaturePipe.h"

namespace vca
{
  namespace icf
  {
    class VCA_API IntegralFeaturesObserver
    {
    public:
      virtual void updateFeatureChannelPointer( map< vca::icf::ChannelType, cv::Mat >& integralMap ) = 0;
    };
  }
}



