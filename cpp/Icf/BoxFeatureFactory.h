#pragma once
#include "FeatureContainer.h"
#include <random>
#include "features/FeaturesLogger.h"
#include "features/FeaturesMacros.h"
namespace vca
{
  namespace icf
  {

    class VCA_API BoxFeatureFactory
    {
    public: 
      struct BoxFeatureConfig
      {
        BoxFeatureConfig()
        {
          useLuv = true;
          useUV = false;
          useGray = false;
          useDxDyMag = false;
          useDxDy = false;
          useQuanDir = true;
          useQuanDirNorm = false;
          useAbsDxDy = false;
          useAbsDxDyMag = false;
          useQuanAbsDxDy = false;
          useQuanDxDy = false;
          useDxDyDxyDyx = false;
          useQuanDirNorm2 = false;

          useLuvForDxDy = false;
          gradientNormSize = 0;

          useOnlySquare = true;
          useGridFeatures = false;
          minSize = 1;
          maxSizeFactor = 1;

          useDxDy1 = false;
          softBin = false;
        }

        bool useLuv, useUV, useGray, useQuanDir, useQuanDirNorm, useQuanDirNorm2;
        bool useDxDyMag, useDxDy, useAbsDxDy, useAbsDxDyMag, useQuanDxDy, useQuanAbsDxDy;
        bool useDxDyDxyDyx;
        bool useLuvForDxDy;
        bool useOnlySquare;
        bool useGridFeatures;
        int minSize;
        float maxSizeFactor;
        int gradientNormSize;

        bool useDxDy1;
        bool softBin;

      };



      BoxFeatureFactory(){}
      BoxFeatureFactory( BoxFeatureConfig& boxFeatCofig );
      void generateFeaturesRandom(FeatureContainer& featcont, int n, int minX, int minY, int maxX, int maxY,
        int minWidth, int minHeight, int maxWidth, int maxHeight, default_random_engine& randomEng );
      void generateFeaturesRandom2(FeatureContainer& featcont, int n, int step, int minX, int minY, int maxX, int maxY,
        int minWidth, int minHeight, int maxWidth, int maxHeight, default_random_engine& randomEng);
      void generateFeaturesGrid( FeatureContainer& featcont, int n, int maxX, int maxY, float paddingFactor, float intersectionFactor);
      void generateFeaturesGrid(FeatureContainer& featcont, int n,  int minX, int minY,
        int maxX, int maxY, float intersectionFactor);
      int getRandomCumIdx(float v, const vector<int>& cumNYXPos);
      void generateFeatures(FeatureContainer& featcont, int n, int minX, int minY, int maxX, int maxY, int minWidth, int minHeight,
        int maxWidth, int maxHeight, default_random_engine& randomEng);
    private:


      vector< ChannelType > usedChannelTypes;   
      int sumBoxWeight;
      int sumBoxNormWeight;
      int dxdyBoxWeight;
      int absdxdyBoxWeight;
      int quandxdyBoxWeight; 
      int dxdydxydyxBoxWeight;
      BoxFeatureConfig boxConf;
    };
  }
}

