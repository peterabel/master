#include "BoxSum.h"


namespace vca
{
  namespace icf
  {
    inline BoxSum::BoxSum() : upLeft_(nullptr), lowLeft_(nullptr) 
    { 

    }


    inline BoxSum::BoxSum( int x_, int y_, int w_, int h_, ChannelType channel_ ) : upLeft_(nullptr), lowLeft_(nullptr)
    { 
      box = ChannelBox(x_, y_, w_, h_, channel_);
    }


    void BoxSum::activateChannels(shared_ptr<IntegralFeatures> feat) const
    {
      feat->activateChannel( box.channel);
    }

    /*write to stream*/
    std::ostream& operator<<(std::ostream& out, const BoxSum& box)
    {
      out << box.box;
      return out;
    }


    /*read from stream*/
    istream& operator>>( istream& in, BoxSum& box )
    {
      in >> box.box;
      return in;
    }

    std::ostream& BoxSum::output(std::ostream& out) const 
    {
      out << *this;
      return out;
    }

    std::istream& BoxSum::input(istream& in)
    {
      in >> *this;
      return in;
    }

    /**/
    inline float BoxSum::eval( void ) const
    {
      sassert( upLeft_ != nullptr && lowLeft_ != nullptr );
      return static_cast<float>(*upLeft_ - upLeft_[box.w] - *lowLeft_ + lowLeft_[box.w]);
    }


    /**/
    inline float BoxSum::eval( const int off ) const
    {
      sassert( upLeft_ != nullptr && lowLeft_ != nullptr );
      return static_cast<float>(upLeft_[off] - upLeft_[off + box.w] - lowLeft_[off] + lowLeft_[off + box.w]);
    }


    /**/
    inline bool BoxSum::set( map< ChannelType, cv::Mat >& integralMap, int y, int x )
    {
      cv::Mat m = integralMap[ box.channel ];
      if( box.y + box.h + y + 1 >= (m.rows) ||  box.x + box.w + x + 1 >= (m.cols) )
      {
        upLeft_ = nullptr;
        lowLeft_ = nullptr;
        return false;
      }
      else
      {
        upLeft_ = m.row( box.y + y + 1 ).ptr< double >() + box.x + x + 1;
        lowLeft_ = m.row( box.y + box.h + y + 1 ).ptr< double >() + box.x + x + 1;
        return true;

      }
    }


    /**/
    scalingFactors BoxSum::scaleFeatBox(IntegralFeatures& integralFeat, BoxSum& tar, const float scaleFactor, bool forceRoundDown /*= false */) const
    {
      float scaledOrigArea = scaleFactor*scaleFactor*box.w * box.h;

      tar = *this;
      scaleBox( tar.box, box, scaleFactor, forceRoundDown);

      tar.set( integralFeat.intImgs_, 0,0);

      float scaledArea = static_cast<float>(tar.box.w * tar.box.h);
      float fixDifferentAreaFactor = scaledOrigArea / scaledArea;

      auto featureScale = integralFeat.pipe->getScalingFactor( tar.box.channel );
      return{ powf(scaleFactor, featureScale), fixDifferentAreaFactor };
      
    }
  }
}