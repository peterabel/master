#pragma once

#include <opencv2/core/core.hpp>
#include "IntegralFeaturesObserver.h"
#include "IntegralFeatures.h"
#include "features/FeaturesLogger.h"
#include "features/FeaturesMacros.h"


namespace vca
{
  namespace icf
  {
    //class VCA_API IntegralFeaturesContainer : IntegralFeaturesObserver
    //{
    //public:
    //  IntegralFeaturesContainer( IntegralFeatures& icf );
    //  ~IntegralFeaturesContainer();

    //  void updateFeatureChannelPointer( map< Vca::Icf::ChannelType, cv::Mat >& integralMap ) override;

    //  FeatureContainer boxes;
    //protected:
    //  IntegralFeatures* icf;
    //};
  }
}



