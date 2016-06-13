#include "MaxParDerAbs.h"


namespace vca
{
  /**
  *
  */

  icf::MaxParDerAbs::MaxParDerAbs()
  {
  }

  /**
  *
  */

  void icf::MaxParDerAbs::calcMap( const cv::Mat& src )
  {
    FEATURES_TRACE( "Calc. feature map " << src.cols << " , " << src.rows << " / " << src.channels() );

    sassert( src.isContinuous() && src.channels() == 3 );


    Vca::match( src, dxp, CV_32FC1 );
    Vca::match( src, dyp, CV_32FC1 );
    Vca::match( src, dxn, CV_32FC1 );
    Vca::match( src, dyn, CV_32FC1 );

    dxp = cv::Mat::zeros( src.size(), CV_32F );
    dyp = cv::Mat::zeros( src.size(), CV_32F ); 
    dxn = cv::Mat::zeros( src.size(), CV_32F );
    dyn = cv::Mat::zeros( src.size(), CV_32F );

    auto ptrDxp = dxp.ptr<float>();
    auto ptrDyp = dyp.ptr<float>();
    auto ptrDxn = dxn.ptr<float>();
    auto ptrDyn = dyn.ptr<float>();


    /*first and last column and row are 0 and never changed, therefore 
      y and x value start and end with one offset */

    #pragma omp parallel for
    for( int y = 1; y < src.rows - 1; y ++ )
    {   
      int offset = y * src.cols;
      const uchar *bgrCen = src.data + 3 * offset;
      const uchar *bgrDown = bgrCen - 3 * src.cols;
      const uchar *bgrUp  = bgrCen + 3 * src.cols;

      ++offset;
   
      for( int x = 3; x <  3 * src.cols - 3; x += 3, ++offset )
      {      
        float dx = maxAbsDiff(   
                             static_cast<float>( bgrCen[x + 5]) - static_cast<float>(bgrCen[ x - 1 ])
                           + static_cast<float>( bgrDown[x + 5]) -static_cast<float>( bgrDown[ x - 1 ])
                           + static_cast<float>( bgrUp[x + 5]) - static_cast<float>(bgrUp[ x - 1 ]),
                             static_cast<float>( bgrCen[x + 4]) - static_cast<float>(bgrCen[ x - 2 ])
                           + static_cast<float>( bgrDown[x + 4]) - static_cast<float>(bgrDown[ x - 2 ])
                           + static_cast<float>( bgrUp[x + 4]) - static_cast<float>(bgrUp[ x - 2 ]),
                             static_cast<float>( bgrCen[x + 3]) - static_cast<float>(bgrCen[ x - 3 ])
                           + static_cast<float>( bgrDown[x + 3]) - static_cast<float>(bgrDown[ x - 3 ])
                           + static_cast<float>(bgrUp[x + 3]) - static_cast<float>(bgrUp[ x - 3 ]) );

        float dy = maxAbsDiff( 
                             static_cast<float>(bgrUp[x - 1]) - static_cast<float>(bgrDown[x - 1])
                           + static_cast<float>(bgrUp[x + 2]) - static_cast<float>(bgrDown[x + 2])
                           + static_cast<float>(bgrUp[x + 5]) - static_cast<float>(bgrDown[x + 5]),
                             static_cast<float>(bgrUp[x - 2]) - static_cast<float>(bgrDown[x - 2])
                           + static_cast<float>(bgrUp[x + 1]) - static_cast<float>(bgrDown[x + 1])
                           + static_cast<float>(bgrUp[x + 4]) - static_cast<float>(bgrDown[x + 4]),
                             static_cast<float>(bgrUp[x - 3]) - static_cast<float>(bgrDown[x - 3])
                           + static_cast<float>(bgrUp[x + 0]) - static_cast<float>(bgrDown[x + 0])
                           + static_cast<float>(bgrUp[x + 3]) - static_cast<float>(bgrDown[x + 3]) );      
        if( dx < 0.f )
        {
          ptrDxp[offset] = 0.f;
          ptrDxn[offset] = -dx;
        }
        else
        {
          ptrDxn[offset] = 0.f;
          ptrDxp[offset] = dx;
        }
        if( dy < 0.f )
        {
          ptrDyp[offset] = 0.f;
          ptrDyn[offset] = -dy;
        }
        else
        {
          ptrDyn[offset] = 0.f;
          ptrDyp[offset] = dy;
        }
        sassert(ptrDxp[offset] >= 0.f);
        sassert(ptrDxn[offset] >= 0.f);
        sassert(ptrDyp[offset] >= 0.f);
        sassert(ptrDyn[offset] >= 0.f);
      }
    }
  }
}


