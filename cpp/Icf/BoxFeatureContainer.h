#pragma once
#include "FeatureChannel.h"
#include "ChannelBox.h"
#include "IntegralFeatures.h"
#include "FeatHandle.h"

namespace vca
{
  namespace icf
  {



    class VCA_API BoxFeatureContainer
    {
    public:
      virtual ~BoxFeatureContainer(){};
      virtual size_t size() = 0;
      virtual bool set( int idx, map< ChannelType, cv::Mat >& integralMap, int x, int y ) = 0;
      virtual float eval( int idx, const uint off = 0 ) const = 0;
      virtual BoxFeature* getBoxFeature( int idx ) = 0;
      virtual float evalScaledBox(shared_ptr<IntegralFeatures> feat, int idx, const float scaleFactor, const uint off  ) const = 0;
      virtual float evalScaledBoxValidRegion(shared_ptr<IntegralFeatures> feat, int idx, const float scaleFactor, const uint off, cv::Rect& validRegion, float invalidReturnValue   ) const = 0;
      virtual FeatHandle addBoxFeature( const BoxFeature& boxFeat) = 0;
      virtual FeatHandle addBoxFeature(std::istream& in ) = 0;
      virtual void activateChannels( shared_ptr<IntegralFeatures> feat, int idx ) = 0;
      virtual scalingFactors scaleFeatBox(shared_ptr<IntegralFeatures> feat, int idx, const float scaleFactor) = 0;
    };

  

    template< class Feat >
    class VCA_API TemplateContainer : public BoxFeatureContainer
    {
    public:
      size_t size() override {return features.size();}
      bool set( int idx,  map< ChannelType, cv::Mat >& integralMap, int x, int y) override {return features[ idx].set( integralMap, x, y );}
      float eval( int idx, const uint off = 0 ) const override { return features[idx].eval( off ); }
      BoxFeature* getBoxFeature( int idx ) override { return static_cast< BoxFeature*>(&features[idx]);}

      void activateChannels( shared_ptr<IntegralFeatures> feat, int idx ) override
      {
        features[idx].activateChannels( feat);
      }

      scalingFactors scaleFeatBox(shared_ptr<IntegralFeatures> feat, int idx, const float scaleFactor) override
      {

        return  features[ idx ].scaleFeatBox( *(feat), features[ idx ], scaleFactor);
      }

      float evalScaledBox(shared_ptr<IntegralFeatures> feat, int idx, const float scaleFactor, const uint off  ) const override
      {
        sassert( idx < features.size());
        Feat scaledBox;
        auto featureScale = features[ idx ].scaleFeatBox( *(feat), scaledBox, scaleFactor);
        if( scaledBox.box.x + scaledBox.box.w >= feat->width() || scaledBox.box.y + scaledBox.box.h  >= feat->height() )
        {
          featureScale = features[ idx ].scaleFeatBox( *(feat), scaledBox,scaleFactor,true);
        }
        sassert( scaledBox.box.x + scaledBox.box.w < feat->width() && scaledBox.box.y + scaledBox.box.h  < feat->height() );

        return scaledBox.eval(off) / featureScale.featureScale*featureScale.scaleCorrection;

      }


      float evalScaledBoxValidRegion(shared_ptr<IntegralFeatures> feat, int idx, const float scaleFactor, const uint off, cv::Rect& validRegion,
        float invalidReturnValue = std::numeric_limits<float>::quiet_NaN() ) const override
      {

        sassert( idx < features.size());
        Feat scaledBox;
        auto featureScale = features[ idx ].scaleFeatBox( *(feat), scaledBox, scaleFactor);
        if( scaledBox.box.x + scaledBox.box.w >= feat->width() || scaledBox.box.y + scaledBox.box.h  >= feat->height() )
        {
          featureScale = features[ idx ].scaleFeatBox( *(feat), scaledBox,scaleFactor,true);
        }

        if(  validRegion.x > scaledBox.box.x 
          || validRegion.y > scaledBox.box.y
          || validRegion.x + validRegion.width <= scaledBox.box.x + scaledBox.box.w
          || validRegion.y + validRegion.height <= scaledBox.box.y + scaledBox.box.h)
          return invalidReturnValue;
        sassert( scaledBox.box.x + scaledBox.box.w < feat->width() && scaledBox.box.y + scaledBox.box.h  < feat->height() );

        return scaledBox.eval(off) / featureScale.featureScale*featureScale.scaleCorrection;


      }


      FeatHandle addBoxFeature( const BoxFeature& boxFeat) override
      {
        auto boxPointer = static_cast< const Feat* >( &boxFeat );
        features.push_back( *boxPointer );
        return FeatHandle(boxPointer->getFeatType(), features.size()-1);
      }
      FeatHandle addBoxFeature(std::istream& in )
      {
        Feat feat;
        in >> feat;
        features.push_back( feat );
        return FeatHandle(feat.getFeatType(), features.size()-1);
      }


    private:
      vector<Feat> features;
    };
  }
}

