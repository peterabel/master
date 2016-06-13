#include "BoxDxDy.h"



namespace vca
{
  namespace icf
  {

    inline BoxDxDy::BoxDxDy() : upLeft_dx_( nullptr ), lowLeft_dx_( nullptr ), upLeft_dy_( nullptr ), lowLeft_dy_( nullptr )
    {
      direction = 0.f;
      useMag = false;
    }


    inline BoxDxDy::BoxDxDy( int x_, int y_, int w_, int h_, ChannelType channel_, float direction_, bool useMag_ )
      : upLeft_dx_( nullptr ), lowLeft_dx_( nullptr ), upLeft_dy_( nullptr ), lowLeft_dy_( nullptr )
    {
     box = ChannelBox(x_, y_, w_, h_, channel_);
      direction = direction_;
      useMag =  useMag_;
    }



    void BoxDxDy::activateChannels(shared_ptr<IntegralFeatures> feat) const
    {

      feat->activateChannel( D_X);
      feat->activateChannel( D_Y);
    }

    /*write to stream*/
    std::ostream& operator<<(std::ostream& out, const BoxDxDy& box)
    {
      out << box.box << " " << box.direction << " " << box.useMag;
      return out;
    }


    /*read from stream*/
    istream& operator>>( istream& in, BoxDxDy& box )
    {

      in >> box.box >> box.direction >> box.useMag;
      return in;
    }


    std::ostream& BoxDxDy::output(std::ostream& out) const 
    {
      out << *this;
      return out;
    }

    std::istream& BoxDxDy::input(istream& in)
    {
      in >> *this;
      return in;
    }



    /**/
    inline float BoxDxDy::eval( const uint off ) const
    {

      sassert( upLeft_dx_ != nullptr && lowLeft_dx_ != nullptr && upLeft_dy_ != nullptr && lowLeft_dy_ != nullptr );
      auto dx = upLeft_dx_[ off ] - upLeft_dx_[ off + box.w ] - lowLeft_dx_[ off ] + lowLeft_dx_[ off + box.w ];
      auto dy = upLeft_dy_[ off ] - upLeft_dy_[ off + box.w ] - lowLeft_dy_[ off ] + lowLeft_dy_[ off + box.w ];

      auto phi = atan2(dy,dx); //phi is between -pi and +pi
      if( phi < 0 )
        phi = phi + M_PI; //rotate 180 degrees, so that phi is in (0,+pi)
      auto delta = abs(direction - phi);
      if( delta > (M_PI_2) )
      {
        if( direction < phi )
          delta = direction + M_PI - phi;
        else
          delta = phi + M_PI - direction;
      }
      if( useMag )
      {
        auto mag = sqrt(dx*dx + dy*dy);
        delta = (mag * delta / M_PI);
      }

      return static_cast<float>(delta);
    
    }


    /**/
    inline bool BoxDxDy::set(  map< ChannelType, cv::Mat >& integralMap, int x, int y )
    {

      cv::Mat dx, dy;
      dx = integralMap[ D_X ];
      dy = integralMap[ D_Y ];

      if( box.y + box.h + y + 1 >= (dx.rows) || box.x + box.w + x + 1 >= (dx.cols)
        || box.y + box.h + y + 1 >= (dy.rows) || box.x + box.w + x + 1 >= (dy.cols))
      {
        upLeft_dx_ = nullptr;
        lowLeft_dx_ = nullptr;
        upLeft_dy_ = nullptr;
        lowLeft_dy_ = nullptr;
        return false;
      }
      else
      {
        upLeft_dx_ = dx.row( box.y + y + 1 ).ptr< double >() + box.x + x + 1;
        lowLeft_dx_ = dx.row( box.y + box.h + y + 1 ).ptr< double >() + box.x + x + 1;
        upLeft_dy_ = dy.row( box.y + y + 1 ).ptr< double >() + box.x + x + 1;
        lowLeft_dy_ = dy.row( box.y + box.h + y + 1 ).ptr< double >() + box.x + x + 1;
        return true;
      }

    }


    /**/
    scalingFactors BoxDxDy::scaleFeatBox(  IntegralFeatures& integralFeat, BoxDxDy& tar, const float scaleFactor, bool forceRoundDown /*= false */ ) const
    {
      tar = *this;
      scaleBox(tar.box, box, scaleFactor, forceRoundDown);

      tar.set(integralFeat.intImgs_, 0, 0);

      float scaledOrigArea = scaleFactor*scaleFactor*box.w * box.h;
      float scaledArea = static_cast<float>(tar.box.w * tar.box.h);
      float fixDifferentAreaFactor = scaledOrigArea / scaledArea;

      if (useMag)
        return{ powf(scaleFactor, integralFeat.pipe->getScalingFactor(MAG)), fixDifferentAreaFactor };
      else
        return{ 1.0, 1.f };

    }
  }
}