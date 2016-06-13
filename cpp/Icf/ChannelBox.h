#pragma once
#include "features/FeaturesLogger.h"
#include "features/FeaturesMacros.h"
#include "FeatureChannel.h"


namespace vca
{
  namespace icf
  {

    struct VCA_API ChannelBox
    {
      ChannelBox( );
      ChannelBox( int x, int y, int w, int h, ChannelType channel );

      friend void scaleBox(  ChannelBox& tar, const ChannelBox& src, float scaleFactor, bool forceRoundDown );

      /*write to stream*/
      friend VCA_API std::ostream& operator<<(std::ostream& out, const ChannelBox& box);

      /*read from stream*/
      friend VCA_API istream& operator>>( istream& in, ChannelBox& box );


      int x, y, w, h;
      ChannelType channel;

    };

  }
}
