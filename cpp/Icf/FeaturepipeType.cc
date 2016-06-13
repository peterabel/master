#include "FeaturepipeType.h"
#include "featureTypeStringConverter.h"

void vca::icf::FeaturepipeType::init(vca::icf::FeaturepipeType::quantDirType qDType, vca::icf::FeaturepipeType::colorType cType, vca::icf::FeaturepipeType::dxdyType dType)
{
  quantdirtype_ = qDType;
  colortype_ = cType;
  dxdytype_ = dType;
  nGradientDir = 6;
  activeChannels.push_back(MAG);

  if (quantdirtype_ != noQuant)
  {
    activeChannels.push_back(DIR_0);
    activeChannels.push_back(DIR_1);
    activeChannels.push_back(DIR_2);
    activeChannels.push_back(DIR_3);
    activeChannels.push_back(DIR_4);
    activeChannels.push_back(DIR_5);
  }

  if (colortype_ == gray)
  {
    activeChannels.push_back(GRAY);
  }
  else
  {
    activeChannels.push_back(LUV_L);
    activeChannels.push_back(LUV_U);
    activeChannels.push_back(LUV_V);
  }

  if (dxdytype_ == nonabs || dxdytype_ == nonabs1 || dxdytype_ == nonabs1luv ||  dxdytype_ == nonabsluv)
  {
    activeChannels.push_back(D_X);
    activeChannels.push_back(D_Y);
  }
  else
  {
    passert(colortype_ == luv); // gray not implemented
    activeChannels.push_back(D_XP);
    activeChannels.push_back(D_YP);
    activeChannels.push_back(D_XN);
    activeChannels.push_back(D_YN);
  }
}
vca::icf::FeaturepipeType::FeaturepipeType(Vca::XmlConf& xmlconfig)
{
  string partDetTypeString, quantDirTypeString, colorTypeString, dxdyTypeString;
  if( xmlconfig.getNodeName() != "featurepipe")
    passert(xmlconfig.setNode("featurepipe"));
  // xmlconfig.get( "partDerType", partDetTypeString);
  passert(xmlconfig.get( "quantDirType", quantDirTypeString));
  passert(xmlconfig.get("colorType", colorTypeString));
  passert(xmlconfig.get("dxdyType", dxdyTypeString));
  passert(xmlconfig.get("normGradientSize", nNormGradientSize));
  passert(xmlconfig.get("softBin", softBin));
  //softBin = false;
  quantdirtype_ = FeatureTypeStringConverter::string2quantDirType(quantDirTypeString);
  colortype_ = FeatureTypeStringConverter::string2colorType(colorTypeString);
  dxdytype_ = FeatureTypeStringConverter::string2dxdyType(dxdyTypeString);
  init(quantdirtype_, colortype_, dxdytype_);
}


//vca::Icf::FeaturepipeType(vca::Icf::FeaturepipeType::quantDirType)// qDType, vca::Icf::FeaturepipeType::colorType cType, vca::Icf::FeaturepipeType::dxdyType dType)
//{
//  cout<< test<< endl;
//  //init(qDType, cType, dType);
//}


void vca::icf::FeaturepipeType::writeConfig(Vca::XmlGen& xmlgen)
{
  xmlgen.putNode("featurepipe");
  string quantDirTypeString = FeatureTypeStringConverter::quantDirType2string(quantdirtype_);
  string colorTypeString = FeatureTypeStringConverter::colorType2string(colortype_);
  string dxdyTypeString = FeatureTypeStringConverter::dxdyType2string(dxdytype_);
  xmlgen.put("quantDirType", quantDirTypeString);
  xmlgen.put("normGradientSize", nNormGradientSize);
  xmlgen.put("softBin", softBin);
  xmlgen.put("colorType", colorTypeString);
  xmlgen.put("dxdyType", dxdyTypeString);
  xmlgen.leaveNode();
}

int vca::icf::FeaturepipeType::nChannels()
{
  return activeChannels.size();
}

vca::icf::FeaturepipeType::FeaturepipeType()
{
  softBin = false;
  nGradientDir = 6;
  nNormGradientSize = 1;
  quantdirtype_ = quantDirMag;
  colortype_ = luv;
  dxdytype_ = nonabs;
}
