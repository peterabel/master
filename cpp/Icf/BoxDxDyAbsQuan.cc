#include "BoxDxDyAbsQuan.h"



namespace vca
{
  namespace icf
  {

    inline BoxDxDyAbsQuan::BoxDxDyAbsQuan()
      : upLeft_dxP_( nullptr ), lowLeft_dxP_( nullptr ), upLeft_dyP_( nullptr ), lowLeft_dyP_( nullptr ),
      upLeft_dxN_( nullptr ), lowLeft_dxN_( nullptr ), upLeft_dyN_( nullptr ), lowLeft_dyN_( nullptr )
    {
      minDirection = 0.f;
      maxDirection = 0.f;
      useAbs = false;
    }


    inline BoxDxDyAbsQuan::BoxDxDyAbsQuan(  int x_, int y_, int w_, int h_, ChannelType channel_, float minDirection_, float maxDirection_,  bool useAbs_ )
      : upLeft_dxP_( nullptr ), lowLeft_dxP_( nullptr ), upLeft_dyP_( nullptr ), lowLeft_dyP_( nullptr ),
      upLeft_dxN_( nullptr ), lowLeft_dxN_( nullptr ), upLeft_dyN_( nullptr ), lowLeft_dyN_( nullptr )
    {
      box = ChannelBox(x_, y_, w_, h_, channel_);
      minDirection = minDirection_;
      maxDirection = maxDirection_;
      useAbs = useAbs_;
    }



    /*write to stream*/
    std::ostream& operator<<(std::ostream& out, const BoxDxDyAbsQuan& box)
    {
      out << box.box << " " << box.minDirection << " " << box.maxDirection << " " << box.useAbs;
      return out;
    }


    /*read from stream*/
    istream& operator>>( istream& in, BoxDxDyAbsQuan& box )
    {

      in >> box.box >> box.minDirection >> box.maxDirection >> box.useAbs;
      return in;
    }


    void BoxDxDyAbsQuan::activateChannels(shared_ptr<IntegralFeatures> feat) const
    {
      feat->activateChannel( D_XP);
      feat->activateChannel( D_YP);
      feat->activateChannel( D_XN);
      feat->activateChannel( D_YN);
    }

    std::ostream& BoxDxDyAbsQuan::output(std::ostream& out) const 
    {
      out << *this;
      return out;
    }

    std::istream& BoxDxDyAbsQuan::input(istream& in)
    {
      in >> *this;
      return in;
    }
    /**/
    inline float BoxDxDyAbsQuan::eval( const uint off ) const
    {

      sassert( upLeft_dxP_ != nullptr && lowLeft_dxP_ != nullptr && upLeft_dyP_ != nullptr && lowLeft_dyP_ != nullptr &&
        upLeft_dxN_ != nullptr && lowLeft_dxN_ != nullptr && upLeft_dyN_ != nullptr && lowLeft_dyN_ != nullptr);

      auto dxP = (upLeft_dxP_[off] + lowLeft_dxP_[off + box.w]) - (upLeft_dxP_[off + box.w] + lowLeft_dxP_[off]);
      auto dxN = (upLeft_dxN_[off] + lowLeft_dxN_[off + box.w]) - (upLeft_dxN_[off + box.w] + lowLeft_dxN_[off]);
      auto dyP = (upLeft_dyP_[off] + lowLeft_dyP_[off + box.w]) - (upLeft_dyP_[off + box.w] + lowLeft_dyP_[off]);
      auto dyN = (upLeft_dyN_[off] - upLeft_dyN_[off + box.w] + lowLeft_dyN_[off + box.w] - lowLeft_dyN_[off]);

      sassert( dxP >= 0.f );
      sassert( dyP >= 0.f );
      sassert( dxN >= 0.f );
      sassert( dyN >= 0.f );
      auto dx = dxP - dxN;
      auto dy = dyP - dyN;
      if( useAbs)
      {
        dx = dxP + dxN;
        dy = dyP + dyN;
      }

      auto phi = - M_PI*.5f;
      if (dxP+dxN+dyP+dyN > .000000001)
      {
        phi = atan2(dy,dx);
        if(useAbs)
        {
          sassert( phi >= 0 );
          sassert( phi <=  M_PI*.5+.000000001);
          if( (dxP<dxN)!=(dyP<dyN))
            phi += M_PI*.5;
        }
        else
        {
          if( phi < 0 )
            phi = phi + M_PI; //rotate 180 degrees, so that phi is in (0,+pi)
        }
      }

      auto mag = 0.;
      if( (minDirection < maxDirection && phi > minDirection && phi <= maxDirection )
        || ( minDirection > maxDirection && (phi > minDirection || phi < maxDirection )))
      {
        mag = sqrt(dx*dx + dy*dy);
      }
      //if( ((phi < minDirection || phi > maxDirection ) && minDirection < maxDirection)
        //|| (phi < minDirection && phi > maxDirection))

      return static_cast<float>(mag);

    }



    /**/
    inline bool BoxDxDyAbsQuan::set(  map< ChannelType, cv::Mat >& integralMap, int y, int x )
    {

      cv::Mat dxp, dyp, dxn, dyn;
      dxp = integralMap[ D_XP ];
      dyp = integralMap[ D_YP ];
      dxn = integralMap[ D_XN ];
      dyn = integralMap[ D_YN ];

      if( box.y + box.h + y + 1 >= (dxp.rows) || box.x + box.w + x + 1 >= (dxp.cols)
        || box.y + box.h + y + 1 >= (dyp.rows) || box.x + box.w + x + 1 >= (dyp.cols)
        || box.y + box.h + y + 1 >= (dxn.rows) || box.x + box.w + x + 1 >= (dxn.cols)
        || box.y + box.h + y + 1 >= (dyn.rows) || box.x + box.w + x + 1 >= (dyn.cols))
      {
        upLeft_dxP_ = nullptr;
        lowLeft_dxP_ = nullptr;
        upLeft_dyP_ = nullptr;
        lowLeft_dyP_ = nullptr;
        upLeft_dxN_ = nullptr;
        lowLeft_dxN_ = nullptr;
        upLeft_dyN_ = nullptr;
        lowLeft_dyN_ = nullptr;
        return false;
      }
      else
      {
        upLeft_dxP_ = dxp.row( box.y + y + 1 ).ptr< double >() + box.x + x + 1;
        lowLeft_dxP_ = dxp.row( box.y + box.h + y + 1 ).ptr< double >() + box.x + x + 1;
        upLeft_dyP_ = dyp.row( box.y + y + 1 ).ptr< double >() + box.x + x + 1;
        lowLeft_dyP_ = dyp.row( box.y + box.h + y + 1 ).ptr< double >() + box.x + x + 1;
        upLeft_dxN_ = dxn.row( box.y + y + 1 ).ptr< double >() + box.x + x + 1;
        lowLeft_dxN_ = dxn.row( box.y + box.h + y + 1 ).ptr< double >() + box.x + x + 1;
        upLeft_dyN_ = dyn.row( box.y + y + 1 ).ptr< double >() + box.x + x + 1;
        lowLeft_dyN_ = dyn.row( box.y + box.h + y + 1 ).ptr< double >() + box.x + x + 1;
        return true;
      }

    }


    /**/
    scalingFactors BoxDxDyAbsQuan::scaleFeatBox(  IntegralFeatures& integralFeat, BoxDxDyAbsQuan& tar, float scaleFactor, bool forceRoundDown /*= false */ ) const
    {
      tar = *this;
      scaleBox( tar.box, box, scaleFactor, forceRoundDown);

      tar.set( integralFeat.intImgs_, 0,0);

      float scaledOrigArea = scaleFactor*scaleFactor*box.w * box.h;
      float scaledArea = static_cast<float>(tar.box.w * tar.box.h);
      float fixDifferentAreaFactor = scaledOrigArea / scaledArea;

      return{ powf(scaleFactor, integralFeat.pipe->getScalingFactor(MAG)), fixDifferentAreaFactor };
  

    }
  }
}



