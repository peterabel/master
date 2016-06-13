#pragma once

#include "BoxSum.h"
#include "BoxDxDy.h"
#include "IntegralFeaturesObserver.h"
#include "IntegralFeatures.h"
#include "BoxDxDyAbs.h"
#include "BoxDxDyAbsQuan.h"
#include "FeatHandle.h"
#include "BoxFeatureContainer.h"


namespace vca
{
  namespace icf
  {
    class VCA_API FeatureContainer: public IntegralFeaturesObserver
    {
    public:

  
      FeatureContainer(shared_ptr<IntegralFeatures> ift);
      ~FeatureContainer();

      int numBoxtypes();
      size_t size( BoxFeature::BoxType boxType );
      int numBoxes() const;

      FeatHandle getBoxIdx( const size_t idx_wavelet ) const;
      const BoxFeature* getBox( const FeatHandle idcs ) const;
      const BoxFeature* getBox( const size_t idx_wavelet ) const;


      void setBox( map< ChannelType, cv::Mat >& integralMap, BoxFeature::BoxType boxType, const size_t idx);
      void setBox( map< ChannelType, cv::Mat >& integralMap, const size_t idx_wavelet);
      void setBox( map< ChannelType, cv::Mat >& integralMap, FeatHandle idxs);

      float evalBox( BoxFeature::BoxType boxType, int idx, const uint off) const;
      float evalBox(  const size_t idx_wavelet, const uint off) const;
      float evalBox( FeatHandle idxs, const uint off) const; 

      float evalScaledBox( const size_t idx_wavelet, const float scaleFactor, const uint off) const;
      float evalScaledBox( FeatHandle idxs,  const float scaleFactor, const uint off) const;
      float evalScaledBox( BoxFeature::BoxType boxType, int idx, const float scaleFactor, const uint off) const;

      float evalBox(BoxFeature::BoxType boxType, int idx, int offx, int offy) const;
      float evalBox(const size_t idx_wavelet, int offx, int offy) const;
      float evalBox(FeatHandle idxs, int offx, int offy) const;

      float evalScaledBox(const size_t idx_wavelet, const float scaleFactor, int offx, int offy) const;
      float evalScaledBox(FeatHandle idxs, const float scaleFactor, int offx, int offy) const;
      float evalScaledBox(BoxFeature::BoxType boxType, int idx, const float scaleFactor, int offx, int offy) const;

      template< class boxt > 
      float evalScaledBoxTemplate( const vector<boxt>& boxs, int idx, const float scaleFactor, const uint off  ) const
      {
        sassert( idx < boxs.size());
        boxt scaledBox;
        float featureScale = boxs[ idx ].scaleFeatBox( *(feat_), scaledBox, scaleFactor);
        if( scaledBox.box.x + scaledBox.box.w >= feat_->width() || scaledBox.box.y + scaledBox.box.h  >= feat_->height() )
        {
          featureScale = boxs[ idx ].scaleFeatBox( *(feat_), scaledBox,scaleFactor,true);
        }
        sassert( scaledBox.box.x + scaledBox.box.w < feat_->width() && scaledBox.box.y + scaledBox.box.h  < feat_->height() );
        return scaledBox.eval( off )/featureScale;

      }

      template<>
      float evalScaledBoxTemplate< BoxSum >(const vector<BoxSum>& boxs, int idx, const float scaleFactor, const uint off) const
      {
        sassert(idx < boxs.size());
        BoxSum scaledBox;
        auto featureScale = boxs[idx].scaleFeatBox(*(feat_), scaledBox, scaleFactor);
        if (scaledBox.box.x + scaledBox.box.w >= feat_->width() || scaledBox.box.y + scaledBox.box.h >= feat_->height())
        {
          featureScale = boxs[idx].scaleFeatBox(*(feat_), scaledBox, scaleFactor, true);
        }
        sassert(scaledBox.box.x + scaledBox.box.w < feat_->width() && scaledBox.box.y + scaledBox.box.h < feat_->height());

        return scaledBox.eval(off) / featureScale.featureScale * featureScale.scaleCorrection;

      }

      float evalScaledBoxValidRegion(const size_t idx_wavelet, const float scaleFactor, const uint off,cv::Rect& validRegion,
        float invalidReturnValue = std::numeric_limits<float>::quiet_NaN() ) const;
      float evalScaledBoxValidRegion(BoxFeature::BoxType boxType, int idx, const float scaleFactor, const uint off,
        cv::Rect& validRegion, float invalidReturnValue = std::numeric_limits<float>::quiet_NaN()) const;
      float evalScaledBoxValidRegion(FeatHandle idxs, const float scaleFactor, const uint off,cv::Rect& validRegion,
        float invalidReturnValue = std::numeric_limits<float>::quiet_NaN() ) const;
      FeatHandle addBoxFeature( const BoxFeature& boxFeat );

      FeatHandle addBoxFeature( std::istream& in, BoxFeature::BoxType featType );

   //   void generateBoxFeatures( BoxFeatureFactory& fabric, int n );

      void addSumBoxFeature( int x, int y, int w, int h, ChannelType channel);

      void addDxdyBoxFeature( int x, int y, int w, int h, float direction, bool useMag);

      void addAbsDxdyBoxFeature(int x, int y, int w, int h, float direction, bool useMag, bool useAbs);
      void addQuanBoxFeature(int x, int y, int w, int h, float minDirection, float maxDirection,  bool useAbs);
      void addSumBoxNormFeature(int x, int y, int width, int height, icf::ChannelType c);

      void updateFeatureChannelPointer( map< vca::icf::ChannelType, cv::Mat >& integralMap ) override;

      int MaxX() const { return maxX; }
      int MaxY() const { return maxY; }

      void recalcMaxBox();

      //friend VCA_API float scaleFeature( const FeatureContainer& srcCont, const size_t srcIdx, FeatureContainer& tar, size_t tarIdx, 
      //  float scaleFactor, bool forceRoundDown = false,bool recalcMaxBox = false);
      scalingFactors scaleFeature(const  BoxFeature::BoxType boxType, const size_t srcIdx, float scaleFactor);
      friend VCA_API void scaleFeatures( const FeatureContainer& srcCont, FeatureContainer& tar, float scaleFactor, bool forceRoundDown = false);

      void copyTo(FeatureContainer& tarCont) const;

      shared_ptr<IntegralFeatures> feat_;

    private:
      void addFeatureBox(int x, int y, int w, int h);
      void addFeatureType(BoxFeature::BoxType boxType);
    private:

      //vector< BoxSum > sumBoxes;
      //vector< BoxDxDy > dxdyBoxes;
      //vector< BoxDxDyAbs > absdxdyBoxes;
      //vector< BoxDxDyAbsQuan > quandxdyBoxes;
      // ... future feature types can be added here
      map< BoxFeature::BoxType, shared_ptr< BoxFeatureContainer > > containers;
      int maxX, maxY;
    };
  }
}