#include "features/Icf/ParDer.h"
#include "utils/Types/ArrUtils.h"

namespace vca
{
  /**
  *
  */

  icf::ParDer::ParDer()
  {
  }

  /**
  *
  */

  void icf::ParDer::calcMap( const cv::Mat& src )
  {


    sassert( src.isContinuous() && src.channels() == 1 && src.depth() == CV_8U );


    Vca::match( src, dx, CV_32FC1 );
    Vca::match( src, dy, CV_32FC1 );

    sassert( dx.size() == src.size() );

    dx = cv::Mat::zeros( src.size(), CV_32F );
    dy = cv::Mat::zeros( src.size(), CV_32F );

    
    auto ptrDx = dx.ptr<float>();
    auto ptrDy = dy.ptr<float>();


    /*first and last column and row are 0 and never changed, therefore 
      y and x value start and end with one offset */

    #pragma omp parallel for
    for( int y = 1; y < static_cast<int>(src.rows - 1); y ++ )
    {   
      int offset = y * src.cols;
      const uchar *bgrCen = src.data +  offset;
      const uchar *bgrDown = bgrCen - src.cols;
      const uchar *bgrUp  = bgrCen + src.cols;

      ++offset;
   
      for( int x = 1; x <  src.cols - 1; x += 1, ++offset )
      {      
        ptrDx[offset] =     
                             static_cast<float>( bgrCen [x + 1]) - static_cast<float>(bgrCen [ x - 1 ])
                           + static_cast<float>( bgrDown[x + 1]) - static_cast<float>(bgrDown[ x - 1 ])
                           + static_cast<float>( bgrUp  [x + 1]) - static_cast<float>(bgrUp  [ x - 1 ]);

        ptrDy[offset] =      static_cast<float>(bgrUp[x - 1]) - static_cast<float>(bgrDown[x - 1])
                           + static_cast<float>(bgrUp[x + 0]) - static_cast<float>(bgrDown[x + 0])
                           + static_cast<float>(bgrUp[x + 1]) - static_cast<float>(bgrDown[x + 1]);      
      }
    }
  }

  void icf::ParDer::calcMapFloat(const cv::Mat& src)
  {


    sassert(src.isContinuous() && src.channels() == 1 && src.depth() == CV_32F);


    Vca::match(src, dx, CV_32FC1);
    Vca::match(src, dy, CV_32FC1);

    sassert(dx.size() == src.size());

    dx = cv::Mat::zeros(src.size(), CV_32F);
    dy = cv::Mat::zeros(src.size(), CV_32F);


    auto ptrDx = dx.ptr<float>();
    auto ptrDy = dy.ptr<float>();


    /*first and last column and row are 0 and never changed, therefore
    y and x value start and end with one offset */

#pragma omp parallel for
    for (int y = 1; y < static_cast<int>(src.rows - 1); y++)
    {
      int offset = y * src.cols;
      const float *bgrCen = src.ptr<float>() +offset;
      const float *bgrDown = bgrCen - src.cols;
      const float *bgrUp = bgrCen + src.cols;

      ++offset;

      for (int x = 1; x < src.cols - 1; x += 1, ++offset)
      {
        ptrDx[offset] =
          static_cast<float>(bgrCen[x + 1]) - static_cast<float>(bgrCen[x - 1])
          + static_cast<float>(bgrDown[x + 1]) - static_cast<float>(bgrDown[x - 1])
          + static_cast<float>(bgrUp[x + 1]) - static_cast<float>(bgrUp[x - 1]);

        ptrDy[offset] = static_cast<float>(bgrUp[x - 1]) - static_cast<float>(bgrDown[x - 1])
          + static_cast<float>(bgrUp[x + 0]) - static_cast<float>(bgrDown[x + 0])
          + static_cast<float>(bgrUp[x + 1]) - static_cast<float>(bgrDown[x + 1]);
      }
    }
  }
}
