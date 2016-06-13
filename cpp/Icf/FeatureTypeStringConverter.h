#pragma once

#include "Featurepipe.h"
#include "BoxFeature.h"
#include "FeaturePipeType.h"
#include "features/FeaturesLogger.h"
#include "features/FeaturesMacros.h"

namespace vca
{
  namespace icf
  {
    class VCA_API FeatureTypeStringConverter
    {
    public:
      static string channel2string( ChannelType c );

      static ChannelType string2channel( string s );

      static string getTypeName( FeaturepipeType pipeType );

    //  static FeaturepipeType getType( string typeName );

      static string boxType2string( BoxFeature::BoxType c );

      static BoxFeature::BoxType string2BoxType( string s );

      static string colorType2string( FeaturepipeType::colorType c );

      static FeaturepipeType::colorType string2colorType( string s );

      static string quantDirType2string( FeaturepipeType::quantDirType c );

      static FeaturepipeType::quantDirType string2quantDirType( string s );
      static string dxdyType2string(FeaturepipeType::dxdyType dxdytype_);
      static FeaturepipeType::dxdyType string2dxdyType(string dxdyTypeString);
    };
  }
}