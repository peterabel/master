#include "features/Icf/FeaturePipeFactory.h"
#include "FeaturePipeGradientsLuv.h"
#include "FeaturePipeGradients.h"
#include "ParDer.h"
#include "MaxParDer.h"
#include "Magnitude.h"
#include "FeaturepipeLuvAbsDxDy.h"
#include "FeaturepipeLuvDxDyDxyDyx.h"
#include "FeaturePipeGradientsUsingLuv.h"
#include "FeaturePipeGradients1Luv.h"
#include "FeaturePipeGradients1Gray.h"
#include "FeaturePipeGradients1UsingLuv.h"


namespace vca
{
  namespace icf
  {

    FeaturepipeFactory::FeaturepipeFactory(Vca::XmlConf& config)
    {
      pipeType = FeaturepipeType( config );
    }

    FeaturepipeFactory::FeaturepipeFactory(FeaturepipeType pipeType_)
    {
      pipeType = pipeType_;
    }


    Featurepipe* FeaturepipeFactory::create(Vca::XmlConf& config)
    {
      auto pipeType = FeaturepipeType(config);
      return create(pipeType);
    }

    Featurepipe* FeaturepipeFactory::create(FeaturepipeType pipeType)
    {
      if (pipeType.colortype_ == FeaturepipeType::luv
        && pipeType.quantdirtype_ == FeaturepipeType::quantDirMag
        && pipeType.dxdytype_ == FeaturepipeType::nonabs)
        return new icf::FeaturePipeGradientsLuv< QuantDirDerMag >(pipeType);

      else if(pipeType.colortype_ == FeaturepipeType::luv
        && pipeType.quantdirtype_ == FeaturepipeType::quantDirMag
        && pipeType.dxdytype_ == FeaturepipeType::nonabs1)
        return new icf::FeaturePipeGradients1Luv< QuantDirDerMag >(pipeType);

      else if((pipeType.colortype_ == FeaturepipeType::gray || pipeType.colortype_ == FeaturepipeType::non)
        && pipeType.quantdirtype_ == FeaturepipeType::quantDirMag
        && pipeType.dxdytype_ == FeaturepipeType::nonabs1)
        return new icf::FeaturePipeGradients1Gray< QuantDirDerMag >(pipeType);


      else  if (pipeType.colortype_ == FeaturepipeType::luv
        && pipeType.quantdirtype_ == FeaturepipeType::quantDirMag
        && pipeType.dxdytype_ == FeaturepipeType::nonabs1luv)
        return new icf::FeaturePipeGradients1UsingLuv< QuantDirDerMag >(pipeType);


      else if (pipeType.colortype_ == FeaturepipeType::luv
        && pipeType.quantdirtype_ == FeaturepipeType::quantDirMag
        && pipeType.dxdytype_ == FeaturepipeType::nonabsluv)
        return new icf::FeaturePipeGradientsUsingLuv< QuantDirDerMag >(pipeType);

      else if ((pipeType.colortype_ == FeaturepipeType::gray || pipeType.colortype_ == FeaturepipeType::non)
        && pipeType.quantdirtype_ == FeaturepipeType::quantDirMag)
        return new icf::FeaturePipeGradient< QuantDirDerMag >(pipeType);

      else if (pipeType.colortype_ == FeaturepipeType::luv
        && pipeType.quantdirtype_ == FeaturepipeType::quantDirNorm
        && pipeType.dxdytype_ == FeaturepipeType::nonabs)
        return new icf::FeaturePipeGradientsLuv< QuantDirDerNorm >(pipeType);

      else if ((pipeType.colortype_ == FeaturepipeType::gray || pipeType.colortype_ == FeaturepipeType::non)
        && pipeType.quantdirtype_ == FeaturepipeType::quantDirNorm)
        return new icf::FeaturePipeGradient< QuantDirDerNorm >(pipeType);

      else if (pipeType.colortype_ == FeaturepipeType::luv
        && pipeType.quantdirtype_ == FeaturepipeType::noQuant
        && pipeType.dxdytype_ == FeaturepipeType::nonabs)
        return new icf::FeaturePipeGradientsLuv< Magnitude >(pipeType);

      else if ((pipeType.colortype_ == FeaturepipeType::gray || pipeType.colortype_ == FeaturepipeType::non)
        && pipeType.quantdirtype_ == FeaturepipeType::noQuant)
        return new icf::FeaturePipeGradient< Magnitude >(pipeType);

      else if (pipeType.colortype_ == FeaturepipeType::luv
        && pipeType.quantdirtype_ == FeaturepipeType::quantDirMag
        && pipeType.dxdytype_ == FeaturepipeType::abs)
        return new icf::FeaturePipeGradientsLuvAbsDxDy< QuantDirDerMag >(pipeType);

      else if (pipeType.colortype_ == FeaturepipeType::luv
        && pipeType.quantdirtype_ == FeaturepipeType::quantDirNorm
        && pipeType.dxdytype_ == FeaturepipeType::abs)
        return new icf::FeaturePipeGradientsLuvAbsDxDy< QuantDirDerNorm >(pipeType);

      else if (pipeType.colortype_ == FeaturepipeType::luv
        && pipeType.quantdirtype_ == FeaturepipeType::noQuant
        && pipeType.dxdytype_ == FeaturepipeType::abs)
        return new icf::FeaturePipeGradientsLuvAbsDxDy< Magnitude >(pipeType);

      else if (pipeType.colortype_ == FeaturepipeType::luv
        && pipeType.quantdirtype_ == FeaturepipeType::noQuant
        && pipeType.dxdytype_ == FeaturepipeType::abs4)
        return new icf::FeaturepipeLuvDxDyDxyDyx(pipeType);
      else
        throw invalid_argument("unknown type");
    }
    

    

    Featurepipe* FeaturepipeFactory::create( ) const 
    {
      return create(pipeType);
    }

    //Featurepipe* FeaturepipeFactory::create(XmlConf& config)
    //{
    //  FeaturepipeType pipetype( config );
    //  return create( pipetype );
    //}

  }
}