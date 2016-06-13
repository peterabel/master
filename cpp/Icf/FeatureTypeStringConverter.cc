#pragma once

#include "FeatureTypeStringConverter.h"

namespace vca
{
  namespace icf
  {
    string FeatureTypeStringConverter::channel2string( ChannelType c )
    {
      if(c==MAG)
      {
        return "MAG";
      }
      else if(c==DIR_0)
      {
        return "DIR_0";
      }
      else if(c==DIR_1)
      {
        return "DIR_1";
      }
      else if(c==DIR_2)
      {
        return "DIR_2";
      }
      else if(c==DIR_3)
      {
        return "DIR_3";
      }
      else if(c==DIR_4)
      {
        return "DIR_4";
      }
      else if(c==DIR_5)
      {
        return "DIR_5";
      }
      else if(c==LUV_L)
      {
        return "LUV_L";
      }
      else if(c==LUV_U)
      {
        return "LUV_U";
      }
      else if(c==LUV_V)
      {
        return "LUV_V";
      }
      else if(c==GRAY)
      {
        return "GRAY";
      }
      else if(c==D_X)
      {
        return "D_X";
      }
      else if(c==D_Y)
      {
        return "D_Y";
      }
      else
      {
        throw invalid_argument("unknown type" );
      }
    }

    ChannelType FeatureTypeStringConverter::string2channel( string s )
    {
      if(s=="MAG")
      {
        return MAG;
      }
      else if(s=="DIR_0")
      {
        return DIR_0;
      }
      else if(s=="DIR_1")
      {
        return DIR_1;
      }
      else if(s=="DIR_2")
      {
        return DIR_2;
      }
      else if(s=="DIR_3")
      {
        return DIR_3;
      }
      else if(s=="DIR_4")
      {
        return DIR_4;
      }
      else if(s=="DIR_5")
      {
        return DIR_5;
      }
      else if(s=="LUV_L")
      {
        return LUV_L;
      }
      else if(s=="LUV_U")
      {
        return LUV_U;
      }
      else if(s=="LUV_V")
      {
        return LUV_V;
      }
      else if(s=="GRAY")
      {
        return GRAY;
      }
      else if(s=="D_X")
      {
        return D_X;
      }
      else if(s=="D_Y")
      {
        return D_Y;
      }
      else
      {
        throw invalid_argument("unknown type" );
      }
    }

    string FeatureTypeStringConverter::getTypeName( FeaturepipeType pipeType )
    {
      string featureTypeString = ""; 
      if( pipeType.quantdirtype_ == FeaturepipeType::noQuant )
      {
        featureTypeString += "noQuant";
      }
      else if( pipeType.quantdirtype_ == FeaturepipeType::quantDirMag )
      {
        featureTypeString += "quantDirMag";
      }
      else if( pipeType.quantdirtype_ == FeaturepipeType::quantDirNorm )
      {
        featureTypeString += "quantDirNorm";
      }

      if( pipeType.colortype_ == FeaturepipeType::gray )
      {
        featureTypeString += "Gray";
      }
      else if( pipeType.colortype_ == FeaturepipeType::luv )
      {
        featureTypeString += "Luv";
      }
      return featureTypeString;
    }

   string FeatureTypeStringConverter::boxType2string(BoxFeature::BoxType c)
    {
      if( c == BoxFeature::sumBox ) return "sumBox";
      else if (c == BoxFeature::sumBoxQuantNorm) return "sumBoxQuantNorm";
      else if( c == BoxFeature::dxdyBox ) return "dxdyBox";
      else if( c == BoxFeature::absdxdyBox ) return "absdxdyBox";
      else if( c == BoxFeature::quandxdyBox ) return "quandxdyBox";
      else if( c == BoxFeature::dxdydxydyxBox ) return "dxdydxydyxBox";
      else  throw invalid_argument("unknown box type" );
    }

    BoxFeature::BoxType FeatureTypeStringConverter::string2BoxType(string s)
    {
      if( s == "sumBox" ) return BoxFeature::sumBox;
      else if (s == "sumBoxQuantNorm") return BoxFeature::sumBoxQuantNorm;
      else if( s == "dxdyBox" ) return BoxFeature::dxdyBox;
      else if( s == "absdxdyBox" ) return BoxFeature::absdxdyBox;
      else if( s == "quandxdyBox" ) return BoxFeature::quandxdyBox;
      else if( s == "dxdydxydyxBox" ) return BoxFeature::dxdydxydyxBox;
      else  throw invalid_argument("unknown box type" );
    }

   string FeatureTypeStringConverter::colorType2string(FeaturepipeType::colorType c)
    {
      if( c == FeaturepipeType::luv ) return "luv";
      else if( c == FeaturepipeType::gray ) return "gray";
      else if (c == FeaturepipeType::non) return "noColor";
      else  throw invalid_argument("unknown color type");
    }


   string FeatureTypeStringConverter::quantDirType2string(FeaturepipeType::quantDirType c)
    {
      if( c == FeaturepipeType::quantDirNorm ) return "quantDirNorm";
      else if( c == FeaturepipeType::quantDirMag ) return "quantDirMag";
      else if( c == FeaturepipeType::noQuant ) return "noQuant";
      else  throw invalid_argument("unknown quantDir type");
    }

    vca::icf::FeaturepipeType::colorType FeatureTypeStringConverter::string2colorType(string s)
    {
      if( s == "luv" ) return FeaturepipeType::luv;
      else if (s == "gray") return FeaturepipeType::gray;
      else if (s == "noColor") return FeaturepipeType::non;
      else  throw invalid_argument("unknown color type" );
    }


    vca::icf::FeaturepipeType::quantDirType FeatureTypeStringConverter::string2quantDirType(string s)
    {
      if( s == "quantDirNorm" ) return FeaturepipeType::quantDirNorm;
      else if( s == "quantDirMag" ) return FeaturepipeType::quantDirMag;
      else if( s == "noQuant" ) return FeaturepipeType::noQuant;
      else  throw invalid_argument("unknown quantDir type" );
    }

   string FeatureTypeStringConverter::dxdyType2string(FeaturepipeType::dxdyType c)
    {
      if( c == FeaturepipeType::abs ) return "abs";
      else if (c == FeaturepipeType::nonabs) return "nonabs";
      else if (c == FeaturepipeType::nonabsluv) return "nonabsluv";
      else if (c == FeaturepipeType::nonabs1) return "nonabs1";
      else if (c == FeaturepipeType::nonabs1luv) return "nonabs1luv";
      else if( c == FeaturepipeType::abs4 ) return "abs4";
      else  throw invalid_argument("unknown type");
    }

    FeaturepipeType::dxdyType FeatureTypeStringConverter::string2dxdyType(string s)
    {
      if( s == "abs" ) return FeaturepipeType::abs;
      else if (s == "nonabs") return FeaturepipeType::nonabs;
      else if (s == "nonabsluv") return FeaturepipeType::nonabsluv;
      else if (s == "nonabs1") return FeaturepipeType::nonabs1;
      else if (s == "nonabs1luv") return FeaturepipeType::nonabs1luv;
      else if( s == "abs4" ) return FeaturepipeType::abs4;
      else  throw invalid_argument("unknown type" );
    }

    //featurePipeType featurePipeFactory::getType( string fpipeTypeName )
    //{
    //  //return boost::lexical_cast<featurePipeType>(fpipeTypeName);

    //  if( fpipeTypeName == getTypeName( grad ) )
    //    return grad;
    //  else if( fpipeTypeName ==  getTypeName( gradLuv ) )
    //    return gradLuv;
    //  else if( fpipeTypeName ==  getTypeName( gradGray ) )
    //    return gradGray;
    //  else if( fpipeTypeName == getTypeName( gradNorm ) )
    //    return gradNorm;
    //  else if( fpipeTypeName ==  getTypeName( gradLuvNorm ) )
    //    return gradLuvNorm;
    //  else if( fpipeTypeName ==  getTypeName( gradGrayNorm ) )
    //    return gradGrayNorm;
    //  else
    //    throw invalid_argument("unknown type name: " + fpipeTypeName );
    //}
  }
}