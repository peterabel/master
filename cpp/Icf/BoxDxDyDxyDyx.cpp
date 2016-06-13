#include "BoxDxDyDxyDyx.h"



namespace vca
{
  namespace icf
  {

    inline BoxDxDyDxyDyx::BoxDxDyDxyDyx() : upLeft_dx_( nullptr ), lowLeft_dx_( nullptr ), upLeft_dy_( nullptr ), lowLeft_dy_( nullptr ),
       upLeft_dxy_( nullptr ), lowLeft_dxy_( nullptr ), upLeft_dyx_( nullptr ), lowLeft_dyx_( nullptr )
    {
      direction = 0.f;
      useMag = false;
    }


    inline BoxDxDyDxyDyx::BoxDxDyDxyDyx( int x_, int y_, int w_, int h_, ChannelType channel_, float direction_, bool useMag_ )
      : upLeft_dx_( nullptr ), lowLeft_dx_( nullptr ), upLeft_dy_( nullptr ), lowLeft_dy_( nullptr ),
      upLeft_dxy_( nullptr ), lowLeft_dxy_( nullptr ), upLeft_dyx_( nullptr ), lowLeft_dyx_( nullptr )
    {
      box = ChannelBox(x_, y_, w_, h_, channel_);
      direction = direction_;
      useMag =  useMag_;
    }



    void BoxDxDyDxyDyx::activateChannels(shared_ptr<IntegralFeatures> feat) const
    {

      feat->activateChannel( D_Xabs);
      feat->activateChannel( D_Yabs);
      feat->activateChannel( D_XYabs);
      feat->activateChannel( D_YXabs);
    }

    /*write to stream*/
    std::ostream& operator<<(std::ostream& out, const BoxDxDyDxyDyx& box)
    {
      out << box.box << " " << box.direction << " " << box.useMag;
      return out;
    }


    /*read from stream*/
    istream& operator>>( istream& in, BoxDxDyDxyDyx& box )
    {

      in >> box.box >> box.direction >> box.useMag;
      return in;
    }


    std::ostream& BoxDxDyDxyDyx::output(std::ostream& out) const 
    {
      out << *this;
      return out;
    }

    std::istream& BoxDxDyDxyDyx::input(istream& in)
    {
      in >> *this;
      return in;
    }



    /**/
    inline float BoxDxDyDxyDyx::eval( const uint off ) const
    {

      sassert( upLeft_dx_ != nullptr && lowLeft_dx_ != nullptr && upLeft_dy_ != nullptr && lowLeft_dy_ != nullptr
        && upLeft_dxy_ != nullptr && lowLeft_dxy_ != nullptr && upLeft_dyx_ != nullptr && lowLeft_dyx_ != nullptr);
      auto dx = upLeft_dx_[ off ] - upLeft_dx_[ off + box.w ] - lowLeft_dx_[ off ] + lowLeft_dx_[ off + box.w ];
      auto dy = upLeft_dy_[ off ] - upLeft_dy_[ off + box.w ] - lowLeft_dy_[ off ] + lowLeft_dy_[ off + box.w ];
      auto dxy = upLeft_dxy_[ off ] - upLeft_dxy_[ off + box.w ] - lowLeft_dxy_[ off ] + lowLeft_dxy_[ off + box.w ];
      auto dyx = upLeft_dyx_[ off ] - upLeft_dyx_[ off + box.w ] - lowLeft_dyx_[ off ] + lowLeft_dyx_[ off + box.w ];

      auto phi1 = atan2(dy,dx); //phi1 is between 0 and +pi/2 (dx, dy >0 )
      auto phi2 = atan2(dyx,dxy); //phi2 is between 0 and +pi/2 (dyx, dxy > 0)
      auto phi =  atan2(phi1,phi2); //phi3 is between 0 and +pi/2 (phi1, phi2 >0 )
      sassert( phi >= 0.f);
      sassert( phi <= M_PI *0.5f );
      sassert( direction >= 0.f);
      sassert( direction <= M_PI *0.5f );
      auto delta = abs(direction - phi);
      //if( delta > (M_PI_2) )
      //{
      //  if( direction < phi )
      //    delta = direction + M_PI - phi;
      //  else
      //    delta = phi + M_PI - direction;
      //}
      //if( useMag )
      //{
      //  auto mag = sqrt(dx*dx + dy*dy);
      //  delta = (mag * delta / M_PI);
      //}

      if( delta > (M_PI_4) )
      {
        if( direction < phi )
          delta = direction + M_PI_2 - phi;
        else
          delta = phi + M_PI_2 - direction;
      }
      sassert( delta >= 0.f);
      sassert( delta <= M_PI_4 );
      if( useMag )
      {
        auto mag = sqrt(dx*dx + dy*dy);
        delta = (mag * delta);
      }

      return static_cast<float>(delta);

    }


    /**/
    inline bool BoxDxDyDxyDyx::set(  map< ChannelType, cv::Mat >& integralMap, int x, int y )
    {

      cv::Mat dx, dy, dxy, dyx;
      dx = integralMap[ D_Xabs ];
      dy = integralMap[ D_Yabs ];
      dxy = integralMap[ D_XYabs ];
      dyx = integralMap[ D_YXabs ];

      if( box.y + box.h + y + 1 >= (dx.rows) || box.x + box.w + x + 1 >= (dx.cols)
        || box.y + box.h + y + 1 >= (dy.rows) || box.x + box.w + x + 1 >= (dy.cols))
      {
        upLeft_dx_ = nullptr;
        lowLeft_dx_ = nullptr;
        upLeft_dy_ = nullptr;
        lowLeft_dy_ = nullptr;
        upLeft_dxy_ = nullptr;
        lowLeft_dxy_ = nullptr;
        upLeft_dyx_ = nullptr;
        lowLeft_dyx_ = nullptr;
        return false;
      }
      else
      {
        upLeft_dx_ = dx.row( box.y + y + 1 ).ptr< double >() + box.x + x + 1;
        lowLeft_dx_ = dx.row( box.y + box.h + y + 1 ).ptr< double >() + box.x + x + 1;
        upLeft_dy_ = dy.row( box.y + y + 1 ).ptr< double >() + box.x + x + 1;
        lowLeft_dy_ = dy.row( box.y + box.h + y + 1 ).ptr< double >() + box.x + x + 1;
        upLeft_dxy_ = dxy.row( box.y + y + 1 ).ptr< double >() + box.x + x + 1;
        lowLeft_dxy_ = dxy.row( box.y + box.h + y + 1 ).ptr< double >() + box.x + x + 1;
        upLeft_dyx_ = dyx.row( box.y + y + 1 ).ptr< double >() + box.x + x + 1;
        lowLeft_dyx_ = dyx.row( box.y + box.h + y + 1 ).ptr< double >() + box.x + x + 1;
        return true;
      }

    }


    /**/
    scalingFactors BoxDxDyDxyDyx::scaleFeatBox(  IntegralFeatures& integralFeat, BoxDxDyDxyDyx& tar, const float scaleFactor, bool forceRoundDown /*= false */ ) const
    {
      tar = *this;
      scaleBox( tar.box, box, scaleFactor, forceRoundDown);

      tar.set( integralFeat.intImgs_, 0,0);
     // tar.box.w*tar.box.h / (scaleFactor*scaleFactor*box.w*box.h);

      float scaledOrigArea = scaleFactor*scaleFactor*box.w * box.h;
      float scaledArea = static_cast<float>(tar.box.w * tar.box.h);
      float fixDifferentAreaFactor = scaledOrigArea / scaledArea;

      if( useMag )
        return{ powf(scaleFactor, integralFeat.pipe->getScalingFactor(MAG)), fixDifferentAreaFactor };
      else
        return{ 1.0, 1.f };

    }
  }
}

