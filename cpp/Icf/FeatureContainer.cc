#include "FeatureContainer.h"
#include "BoxDxDyDxyDyx.h"
#include "BoxSumQuantNorm.h"

namespace vca
{
  namespace icf
  {
    /******************/
    /*FeatureContainer*/
    /******************/
    icf::FeatureContainer::FeatureContainer(shared_ptr<icf::IntegralFeatures> ift): feat_( ift )
    {
       feat_->registerObserver( this);
       maxX = maxY = 0;
    }

    icf::FeatureContainer::~FeatureContainer()
    {
      feat_->deregisterObserver( this ); 

    }

    int icf::FeatureContainer::numBoxtypes()
    {
      return containers.size();
    }
    inline bool correctType( BoxFeature::BoxType boxType )
     {
       return (boxType == BoxFeature::sumBox
         || boxType == BoxFeature::sumBoxQuantNorm
         || boxType == BoxFeature::dxdyBox
         || boxType == BoxFeature::absdxdyBox
         || boxType == BoxFeature::quandxdyBox
         || boxType == BoxFeature::dxdydxydyxBox);
     }


    size_t icf::FeatureContainer::size(BoxFeature::BoxType boxType)
    {
      sassert(correctType( boxType ));
      return containers[boxType]->size();
    }

    int icf::FeatureContainer::numBoxes() const
    {
      int n = 0;
      for( auto it = containers.begin(); it!= containers.end(); ++it)
      {
        n += it->second->size();
      }
      return n;
    }


    /*return featureType and featureIdx for a specific idx of the (transposed) training matrix*/
    FeatHandle icf::FeatureContainer::getBoxIdx( const size_t idx_wavelet ) const
    {
      FeatHandle idxPair;
      sassert( idx_wavelet >= 0 && idx_wavelet < numBoxes() );
      int n = 0;
      for( auto it = containers.begin(); it!= containers.end(); ++it)
      {
        auto curSize = it->second->size();
        if( idx_wavelet < n + curSize )
        {
          idxPair.featType = it->first;
          idxPair.featIdx = idx_wavelet - n;
          break;
        }
        n += curSize;

      }
      return idxPair;
    }


    void icf::FeatureContainer::setBox(map< ChannelType, cv::Mat >& integralMap, const size_t idx_wavelet)
    {
      setBox(integralMap, getBoxIdx( idx_wavelet ));
    }

    void icf::FeatureContainer::setBox(map< ChannelType, cv::Mat >& integralMap, FeatHandle idxs )
    {
      setBox(integralMap, idxs.featType, idxs.featIdx);
    }

    void icf::FeatureContainer::setBox( map< ChannelType, cv::Mat >& integralMap, BoxFeature::BoxType boxType, const size_t idx)
    {
      sassert(correctType( boxType ));
      sassert( idx < containers[ boxType ]->size())
      containers[boxType]->set( idx, integralMap, 0, 0);
    }

    float icf::FeatureContainer::evalBox(const size_t idx_wavelet,  const uint off) const
    {
      return evalBox( getBoxIdx( idx_wavelet ),off);
    }

    float icf::FeatureContainer::evalBox( FeatHandle idxs, const uint off ) const
    {
      return evalBox( idxs.featType, idxs.featIdx, off);
    }

    float icf::FeatureContainer::evalBox(BoxFeature::BoxType boxType, int idx, const uint off) const
    {
      sassert(correctType( boxType ));
      sassert( idx <  containers.at(boxType )->size())
      return containers.at(boxType)->eval( idx, off);
    }


    void FeatureContainer::addFeatureBox(int x, int y, int w, int h)
    {

      maxX = max( maxX, x + w );
      maxY = max( maxY, y + h );
    }


    void FeatureContainer::addFeatureType(BoxFeature::BoxType boxType)
    {
      sassert( correctType( boxType ));
      if( containers.count(boxType) > 0 )
        return;

      if(boxType == BoxFeature::sumBox )
        containers[boxType] = shared_ptr<BoxFeatureContainer>( new TemplateContainer<BoxSum>());
      else if (boxType == BoxFeature::sumBoxQuantNorm)
        containers[boxType] = shared_ptr<BoxFeatureContainer>(new TemplateContainer<BoxSumQuantNorm>());
      else if(boxType == BoxFeature::dxdyBox )
        containers[boxType] = shared_ptr<BoxFeatureContainer>( new TemplateContainer<BoxDxDy>());
      else if(boxType == BoxFeature::absdxdyBox )
        containers[boxType] = shared_ptr<BoxFeatureContainer>( new TemplateContainer<BoxDxDyAbs>());
      else if(boxType == BoxFeature::quandxdyBox )
        containers[boxType] = shared_ptr<BoxFeatureContainer>( new TemplateContainer<BoxDxDyAbsQuan>());
      else if(boxType == BoxFeature::dxdydxydyxBox)
        containers[boxType] = shared_ptr<BoxFeatureContainer>( new TemplateContainer<BoxDxDyDxyDyx>());

    }

    void FeatureContainer::copyTo(FeatureContainer& tarCont) const
    {
      tarCont.containers.clear();
      tarCont.feat_ = feat_;
      tarCont.maxX = maxX;
      tarCont.maxY = maxY;
      for( auto it = containers.begin(); it!= containers.end(); ++it)
      {
        tarCont.addFeatureType( it->first);
        int n = it->second->size();
        for( auto j = 0; j < n; j++)
        {
          auto tmp = it->second->getBoxFeature(j);
          tarCont.addBoxFeature( *tmp );
        }
      }

    }

    const BoxFeature* FeatureContainer::getBox(const FeatHandle idcs) const
    {
      return containers.at(idcs.featType)->getBoxFeature(idcs.featIdx);
    }

    const BoxFeature* FeatureContainer::getBox(const size_t idx_wavelet) const
    {
      return getBox( getBoxIdx(idx_wavelet));
    }



    float FeatureContainer::evalScaledBox(const size_t idx_wavelet, const float scaleFactor, const uint off) const
    {
      return evalScaledBox( getBoxIdx( idx_wavelet ), scaleFactor, off );
    }

    float FeatureContainer::evalScaledBox(FeatHandle idxs, const float scaleFactor, const uint off) const
    {
      return evalScaledBox( idxs.featType, idxs.featIdx, scaleFactor, off );
    }

    float FeatureContainer::evalScaledBox(BoxFeature::BoxType boxType, int idx, const float scaleFactor, const uint off) const
    {
      return containers.at(boxType)->evalScaledBox(feat_,  idx, scaleFactor, off  );
    }

    float FeatureContainer::evalScaledBox(const size_t idx_wavelet, const float scaleFactor, int offx, int offy) const
    {
      int off = feat_->getOffset(offx, offy);
      return evalScaledBox(idx_wavelet, scaleFactor, off);
    }

    float FeatureContainer::evalScaledBox(FeatHandle idxs, const float scaleFactor, int offx, int offy) const
    {
      uint off = feat_->getOffset(offx, offy);
      return evalScaledBox(idxs, scaleFactor, off);
    }

    float FeatureContainer::evalScaledBox(BoxFeature::BoxType boxType, int idx, const float scaleFactor, int offx, int offy) const
    {
      uint off = feat_->getOffset(offx, offy);
      return evalScaledBox(boxType, idx, scaleFactor, off);
    }

    float FeatureContainer::evalScaledBoxValidRegion(const size_t idx_wavelet, const float scaleFactor, const uint off, cv::Rect& validRegion,
      float invalidReturnValue  ) const
    {
      return evalScaledBoxValidRegion( getBoxIdx( idx_wavelet ), scaleFactor, off, validRegion, invalidReturnValue );
    }

    float FeatureContainer::evalScaledBoxValidRegion(FeatHandle idxs, const float scaleFactor, const uint off, cv::Rect& validRegion,
      float invalidReturnValue) const
    {
      return evalScaledBoxValidRegion( idxs.featType, idxs.featIdx, scaleFactor, off, validRegion, invalidReturnValue);
    }

    float FeatureContainer::evalScaledBoxValidRegion(BoxFeature::BoxType boxType, int idx, const float scaleFactor, const uint off, cv::Rect& validRegion,
      float invalidReturnValue) const
    {
      return containers.at(boxType)->evalScaledBoxValidRegion(feat_,  idx, scaleFactor, off, validRegion, invalidReturnValue );
    }

    float FeatureContainer::evalBox(BoxFeature::BoxType boxType, int idx, int offx, int offy) const
    {
      uint off = feat_->getOffset(offx, offy);
      return evalBox(boxType, idx, off);
    }

    float FeatureContainer::evalBox(FeatHandle idxs, int offx, int offy) const
    {
      uint off = feat_->getOffset(offx, offy);
      return evalBox(idxs, off);
    }

    float FeatureContainer::evalBox(const size_t idx_wavelet, int offx, int offy) const
    {
      uint off = feat_->getOffset(offx, offy);
      return evalBox(idx_wavelet, off);
    }




    FeatHandle FeatureContainer::addBoxFeature( const BoxFeature& boxFeat)
    {
      if( containers.count(boxFeat.getFeatType()) == 0 )
        addFeatureType( boxFeat.getFeatType());
      boxFeat.activateChannels( feat_ );
      auto tmp = boxFeat.getFeatType();
      sassert(containers.count(tmp) > 0);
      auto idcs = containers[tmp]->addBoxFeature(boxFeat);
      addFeatureBox( boxFeat.box.x, boxFeat.box.y, boxFeat.box.w, boxFeat.box.h );
      return idcs;
    }

    FeatHandle FeatureContainer::addBoxFeature(std::istream& in, BoxFeature::BoxType featType)
    {
      if( containers.count(featType) == 0 )
        addFeatureType( featType);
      auto idcs = containers[ featType ]->addBoxFeature( in );
      containers[idcs.featType]->activateChannels( feat_, idcs.featIdx );
      auto boxFeat = getBox( idcs );
      addFeatureBox( boxFeat->box.x, boxFeat->box.y, boxFeat->box.w, boxFeat->box.h );
      return idcs;
    }

    void FeatureContainer::addSumBoxFeature(int x, int y, int w, int h, ChannelType channel)
    {
      addBoxFeature( BoxSum(x,y, w, h, channel ));
    }

    void FeatureContainer::addDxdyBoxFeature(int x, int y, int w, int h, float direction, bool useMag)
    {
      addBoxFeature( BoxDxDy(x,y, w, h, D_X, direction, useMag));
    }

    void FeatureContainer::addAbsDxdyBoxFeature(int x, int y, int w, int h, float direction, bool useMag, bool useAbs)
    {
      addBoxFeature( BoxDxDyAbs(x,y, w, h, D_X, direction, useMag, useAbs));
    }

    void FeatureContainer::addQuanBoxFeature(int x, int y, int w, int h, float minDirection, float maxDirection, bool useAbs)
    {
      addBoxFeature(  BoxDxDyAbsQuan(x,y, w, h, D_X, minDirection, maxDirection, useAbs));
    }

    void FeatureContainer::addSumBoxNormFeature(int x, int y, int width, int height, icf::ChannelType c)
    {
      addBoxFeature(BoxSumQuantNorm(x, y, width, height, c));
    }

    void FeatureContainer::updateFeatureChannelPointer(map< vca::icf::ChannelType, cv::Mat >& integralMap)
    {
      for( auto it = containers.begin(); it!= containers.end(); ++it)
      {
        int n = it->second->size();
        for( auto j = 0; j < n; j++)
        {
          it->second->set(j,integralMap,0,0);
        }
      }
    }

    void FeatureContainer::recalcMaxBox()
    {
      maxX = 0; 
      maxY = 0;
      for( auto i = 0; i < numBoxes(); ++i )
      {
        auto boxFeat = getBox( i );
        maxX = max( maxX, boxFeat->box.x + boxFeat->box.w);
        maxY = max( maxY, boxFeat->box.y + boxFeat->box.h);
      }
    }


    scalingFactors FeatureContainer::scaleFeature(const  BoxFeature::BoxType boxType, const size_t idx, float scaleFactor)
    {

      scalingFactors featureScale = { 1.f, 1.f };
      sassert( idx < containers[boxType]->size());

      featureScale = containers[boxType]->scaleFeatBox( feat_, idx, scaleFactor);

      return featureScale;
    }


    void scaleFeatures(const FeatureContainer& srcCont, FeatureContainer& tarCont, float scaleFactor, bool forceRoundDown)
    {
      srcCont.copyTo(tarCont);
       for( auto it = srcCont.containers.begin(); it!= srcCont.containers.end(); ++it)
       {
         int n = it->second->size();
         for( auto j = 0; j < n; j++)
         {
            it->second->scaleFeatBox( tarCont.feat_, j, scaleFactor );
         }
       }
    }
  }
}