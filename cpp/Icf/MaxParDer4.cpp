#include "MaxParDer4.h"


namespace vca
{
  /**
  *
  */

  icf::MaxParDer4::MaxParDer4()
  {
  }

  /**
  *
  */

  void icf::MaxParDer4::calcMap( const cv::Mat& src )
  {
    FEATURES_TRACE( "Calc. feature map " << src.cols << " , " << src.rows << " / " << src.channels() );

    sassert( src.isContinuous() && src.channels() == 3 );


    Vca::match( src, dx, CV_32FC1 );
    Vca::match( src, dy, CV_32FC1 );
    Vca::match( src, dyx, CV_32FC1 );
    Vca::match( src, dxy, CV_32FC1 );

    sassert( dx.size() == src.size() );

    dx = cv::Mat::zeros( src.size(), CV_32F );
    dy = cv::Mat::zeros( src.size(), CV_32F );
    dxy = cv::Mat::zeros( src.size(), CV_32F );
    dyx = cv::Mat::zeros( src.size(), CV_32F );

    auto ptrDx = dx.ptr<float>();
    auto ptrDy = dy.ptr<float>();
    auto ptrDxy = dxy.ptr<float>();
    auto ptrDyx = dyx.ptr<float>();


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
        ptrDx[offset] = maxAbsDiff(   
                             static_cast<float>( bgrCen[x + 5]) - static_cast<float>(bgrCen[ x - 1 ])
                           + static_cast<float>( bgrDown[x + 5]) -static_cast<float>( bgrDown[ x - 1 ])
                           + static_cast<float>( bgrUp[x + 5]) - static_cast<float>(bgrUp[ x - 1 ]),
                             static_cast<float>( bgrCen[x + 4]) - static_cast<float>(bgrCen[ x - 2 ])
                           + static_cast<float>( bgrDown[x + 4]) - static_cast<float>(bgrDown[ x - 2 ])
                           + static_cast<float>( bgrUp[x + 4]) - static_cast<float>(bgrUp[ x - 2 ]),
                             static_cast<float>( bgrCen[x + 3]) - static_cast<float>(bgrCen[ x - 3 ])
                           + static_cast<float>( bgrDown[x + 3]) - static_cast<float>(bgrDown[ x - 3 ])
                           + static_cast<float>(bgrUp[x + 3]) - static_cast<float>(bgrUp[ x - 3 ]) );

        ptrDy[offset] = maxAbsDiff( 
                             static_cast<float>(bgrUp[x - 1]) - static_cast<float>(bgrDown[x - 1])
                           + static_cast<float>(bgrUp[x + 2]) - static_cast<float>(bgrDown[x + 2])
                           + static_cast<float>(bgrUp[x + 5]) - static_cast<float>(bgrDown[x + 5]),
                             static_cast<float>(bgrUp[x - 2]) - static_cast<float>(bgrDown[x - 2])
                           + static_cast<float>(bgrUp[x + 1]) - static_cast<float>(bgrDown[x + 1])
                           + static_cast<float>(bgrUp[x + 4]) - static_cast<float>(bgrDown[x + 4]),
                             static_cast<float>(bgrUp[x - 3]) - static_cast<float>(bgrDown[x - 3])
                           + static_cast<float>(bgrUp[x + 0]) - static_cast<float>(bgrDown[x + 0])
                           + static_cast<float>(bgrUp[x + 3]) - static_cast<float>(bgrDown[x + 3]) );      

        ptrDxy[offset] = maxAbsDiff(   
            static_cast<float>( bgrUp[x - 1]) + static_cast<float>( bgrUp[x + 2])
          + static_cast<float>( bgrCen[x - 1]) - static_cast<float>(bgrCen[ x + 5])
          - static_cast<float>( bgrDown[ x + 2 ]) - static_cast<float>( bgrDown[ x + 5 ]),

          static_cast<float>( bgrUp[x - 2]) + static_cast<float>( bgrUp[x + 1])
          + static_cast<float>( bgrCen[x - 2]) - static_cast<float>(bgrCen[ x + 4])
          - static_cast<float>( bgrDown[ x + 1 ]) - static_cast<float>( bgrDown[ x + 4 ]),
          
          static_cast<float>( bgrUp[x - 3]) + static_cast<float>( bgrUp[x + 0])
          + static_cast<float>( bgrCen[x - 3]) - static_cast<float>(bgrCen[ x + 3])
          - static_cast<float>( bgrDown[ x - 0]) - static_cast<float>( bgrDown[ x + 3 ]));

        ptrDyx[offset] = maxAbsDiff( 
          static_cast<float>( bgrUp[x + 2]) + static_cast<float>( bgrUp[x + 5])
          - static_cast<float>( bgrCen[x - 1]) + static_cast<float>(bgrCen[ x + 5])
          - static_cast<float>( bgrDown[ x - 1 ]) - static_cast<float>( bgrDown[ x + 2 ]),

          static_cast<float>( bgrUp[x + 1]) + static_cast<float>( bgrUp[x + 4])
          - static_cast<float>( bgrCen[x - 2]) + static_cast<float>(bgrCen[ x + 4])
          - static_cast<float>( bgrDown[ x - 2 ]) - static_cast<float>( bgrDown[ x + 1 ]),

          static_cast<float>( bgrUp[x + 0]) + static_cast<float>( bgrUp[x + 3])
          - static_cast<float>( bgrCen[x - 3]) + static_cast<float>(bgrCen[ x + 3])
          - static_cast<float>( bgrDown[ x - 3 ]) - static_cast<float>( bgrDown[ x - 0]));

        ptrDx[offset] = abs( ptrDx[offset]);
        ptrDy[offset] = abs( ptrDy[offset]);
        ptrDxy[offset] = abs( ptrDxy[offset]);
        ptrDyx[offset] = abs( ptrDyx[offset]);

      }
    }
  }
}


