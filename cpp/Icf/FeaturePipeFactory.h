/**
*
* Author        : --
*
* Module        : Features
* Classes       : FeaturePipe
* Namespace     : vca::Icf
*
* Snippets      : --
* Resources     : --
*
* Description   : Calculate single feature maps and return integral images
*
* Issues        : - Variable feature configuration missing
*
*/

#pragma once

#include "features/Icf/FeaturePipe.h"
#include "utils/XmlConf.h"
#include "features/FeaturesLogger.h"
#include "features/FeaturesMacros.h"


namespace vca
{

  namespace icf
  {

    class VCA_API FeaturepipeFactory
    {
    public:

      FeaturepipeFactory( Vca::XmlConf& config);
      FeaturepipeFactory( FeaturepipeType pipeType_);

      static Featurepipe* create(Vca::XmlConf& config);
      static Featurepipe* create(FeaturepipeType pipeType_);

      Featurepipe* create(  ) const;
           
      FeaturepipeType pipeType;
    };
    
  }

}
