#pragma once
#include "utils\XmlConf.h"
#include "utils\XmlGen.h"
#include "features/FeaturesLogger.h"
#include "features/FeaturesMacros.h"
#include "FeatureChannel.h"

namespace vca
{

  namespace icf
  {

    struct VCA_API FeaturepipeType
    {

      //enum partDerType{maxParDer, partDer};
      enum quantDirType{quantDirNorm, quantDirMag, noQuant};
      enum colorType{luv,gray, non};
      enum dxdyType{ abs, nonabs, abs4, nonabsluv, nonabs1, nonabs1luv };

      FeaturepipeType( );
      FeaturepipeType(Vca::XmlConf& xmlconfig);
      FeaturepipeType(quantDirType qDType, colorType cType, dxdyType dType){
        init(qDType, cType, dType);
      }
      
      void writeConfig( Vca::XmlGen& xmlgen );

      int nChannels();

      int nGradientDir;
      int nNormGradientSize;
      bool softBin;
     // partDerType partdertype_;
      quantDirType quantdirtype_;
      colorType colortype_;
      dxdyType dxdytype_;

      vector< ChannelType > activeChannels;
    private:
      void init(quantDirType qDType, colorType cType, dxdyType dType);
    };

    inline bool operator==(const FeaturepipeType& lhs, const FeaturepipeType& rhs)
    { 
      return (lhs.nGradientDir == rhs.nGradientDir
        && lhs.quantdirtype_ == rhs.quantdirtype_
        && lhs.colortype_ == rhs.colortype_
         && lhs.dxdytype_ == rhs.dxdytype_
         && lhs.nGradientDir == rhs.nGradientDir
         && lhs.nNormGradientSize == rhs.nNormGradientSize
         && lhs.softBin == rhs.softBin);
    }

    inline bool operator<(const FeaturepipeType& lhs, const FeaturepipeType& rhs)
    {
      return (lhs.nGradientDir == rhs.nGradientDir
        && lhs.quantdirtype_ == rhs.quantdirtype_
        && (lhs.colortype_ == rhs.colortype_ || (lhs.colortype_ == FeaturepipeType::colorType::non))
        && lhs.dxdytype_ == rhs.dxdytype_
        && lhs.nGradientDir == rhs.nGradientDir
        && lhs.nNormGradientSize == rhs.nNormGradientSize
        && lhs.softBin == rhs.softBin);
    }

    inline bool operator!=(const FeaturepipeType& lhs, const FeaturepipeType& rhs){return !(lhs == rhs);}
  }
}