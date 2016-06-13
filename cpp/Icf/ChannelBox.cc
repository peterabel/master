#include "ChannelBox.h"
#include "featureTypeStringConverter.h"

namespace vca
{
  namespace icf
  {

    ChannelBox::ChannelBox( ) : x(0), y(0), w(0), h(0), channel(MAG){}

    ChannelBox::ChannelBox( int x_, int y_, int w_, int h_, ChannelType channel_ )
      : x(x_),y(y_),w(w_),h(h_),channel(channel_){}

    void scaleBox(  ChannelBox& tar, const ChannelBox& src, float scaleFactor, bool forceRoundDown )
    {
      tar.channel = src.channel;

      auto srcx2 = src.x + src.w;
      auto srcy2 = src.y + src.h;

      tar.x = (int)(src.x * scaleFactor);
      tar.y = (int)(src.y * scaleFactor);
      tar.h = (int)(srcy2 * scaleFactor) - tar.y;
      tar.w = (int)(srcx2 * scaleFactor) - tar.x;
      ////scale box
      //if( forceRoundDown )
      //{
      //  tar.x = (int)( src.x * scaleFactor );
      //  tar.y = (int)( src.y * scaleFactor );
      //  tar.h = (int)( src.h * scaleFactor );
      //  tar.w = (int)( src.w * scaleFactor );
      //}
      //else
      //{
      //  tar.x = (int)( src.x * scaleFactor + 0.5f );
      //  tar.y = (int)( src.y * scaleFactor  + 0.5f );
      //  tar.h = (int)( src.h * scaleFactor  + 0.5f );
      //  tar.w = (int)( src.w * scaleFactor  + 0.5f );
      //}

    }

    /*write to stream*/
    std::ostream& operator<<(std::ostream& out, const ChannelBox& box)
    {
      out << box.x << "  " << box.y << "  " << box.w << "  " << box.h << "  " << FeatureTypeStringConverter::channel2string( box.channel );
      return out;
    }


    /*read from stream*/
    istream& operator>>( istream& in, ChannelBox& box )
    {
      string channel_str;
      in >> box.x >> box.y >> box.w >> box.h >> channel_str;
      box.channel = FeatureTypeStringConverter::string2channel( channel_str );
      return in;
    }


  }
}


