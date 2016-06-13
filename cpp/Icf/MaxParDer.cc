#include "features/Icf/MaxParDer.h"
#include "utils/Types/ArrUtils.h"

namespace vca
{
  /**
  *
  */

  icf::MaxParDer::MaxParDer()
  {
  }

  /**
  *
  */

  void icf::MaxParDer::calcMap( const cv::Mat& src )
  {
//    FEATURES_TRACE( "Calc. feature map " << src.cols << " , " << src.rows << " / " << src.channels() );

    sassert( src.isContinuous());
    sassert( src.channels() == 3 );


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
    for( int y = 1; y < src.rows - 1; y ++ )
    {   
      int offset = y * src.cols;
      const uchar *bgrCen = src.data + 3 * offset;
      const uchar *bgrDown = bgrCen - 3 * src.cols;
      const uchar *bgrUp  = bgrCen + 3 * src.cols;
	
	  float tmp0b = 0;
	  float tmp1b = static_cast<float>(bgrUp[0]) - static_cast<float>(bgrDown[0]);
	  float tmp2b = static_cast<float>(bgrUp[3]) - static_cast<float>(bgrDown[3]);
	  float tmp0g = 0;
	  float tmp1g = static_cast<float>(bgrUp[1]) - static_cast<float>(bgrDown[1]);
	  float tmp2g = static_cast<float>(bgrUp[4]) - static_cast<float>(bgrDown[4]);
	  float tmp0r = 0;
	  float tmp1r = static_cast<float>(bgrUp[2]) - static_cast<float>(bgrDown[2]);
	  float tmp2r = static_cast<float>(bgrUp[5]) - static_cast<float>(bgrDown[5]);
	  ++offset;

      for( int x = 3; x <  3 * src.cols - 3; x += 3, ++offset )
      {      
		tmp0r = tmp1r;
		tmp0g = tmp1g;
		tmp0b = tmp1b;
		tmp1r = tmp2r;
		tmp1g = tmp2g;
		tmp1b = tmp2b;
		tmp2r = static_cast<float>(bgrUp[x + 5]) - static_cast<float>(bgrDown[x + 5]);
		tmp2g = static_cast<float>(bgrUp[x + 4]) - static_cast<float>(bgrDown[x + 4]);
		tmp2b = static_cast<float>(bgrUp[x + 3]) - static_cast<float>(bgrDown[x + 3]);

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
			tmp0r + tmp1r + tmp2r,
			tmp0g + tmp1g + tmp2g,
			tmp0b + tmp1b + tmp2b);

/*        ptrDy[offset] = maxAbsDiff( 
                             static_cast<float>(bgrUp[x - 1]) - static_cast<float>(bgrDown[x - 1])
                           + static_cast<float>(bgrUp[x + 2]) - static_cast<float>(bgrDown[x + 2])
                           + static_cast<float>(bgrUp[x + 5]) - static_cast<float>(bgrDown[x + 5]),
                             static_cast<float>(bgrUp[x - 2]) - static_cast<float>(bgrDown[x - 2])
                           + static_cast<float>(bgrUp[x + 1]) - static_cast<float>(bgrDown[x + 1])
                           + static_cast<float>(bgrUp[x + 4]) - static_cast<float>(bgrDown[x + 4]),
                             static_cast<float>(bgrUp[x - 3]) - static_cast<float>(bgrDown[x - 3])
                           + static_cast<float>(bgrUp[x + 0]) - static_cast<float>(bgrDown[x + 0])
                           + static_cast<float>(bgrUp[x + 3]) - static_cast<float>(bgrDown[x + 3]) );    */  
      }
    }
  }

  void icf::MaxParDer::calcMap(const cv::Mat& src1, const cv::Mat& src2, const cv::Mat& src3 )
  {
    //    FEATURES_TRACE( "Calc. feature map " << src.cols << " , " << src.rows << " / " << src.channels() );

    sassert(src1.isContinuous());
    sassert(src2.isContinuous());
    sassert(src3.isContinuous());
    sassert(src1.channels() == 1);
    sassert(src2.channels() == 1);
    sassert(src3.channels() == 1);

    sassert(src1.size() == src2.size())
    sassert(src1.size() == src3.size())

    Vca::match(src1, dx, CV_32FC1);
    Vca::match(src1, dy, CV_32FC1); 



    dx = cv::Mat::zeros(src1.size(), CV_32F);
    dy = cv::Mat::zeros(src1.size(), CV_32F);

    auto ptrDx = dx.ptr<float>();
    auto ptrDy = dy.ptr<float>();


    /*first and last column and row are 0 and never changed, therefore
    y and x value start and end with one offset */

#pragma omp parallel for
    for (int y = 1; y < src1.rows - 1; y++)
    {
      int offset = y * src1.cols;
      auto cen1 = src1.ptr<float>() + offset;
      auto down1 = cen1 - src1.cols;
      auto up1 = cen1 + src1.cols;

      auto cen2 = src2.ptr<float>() +offset;
      auto down2 = cen2 - src2.cols;
      auto up2 = cen2 + src2.cols;

      auto cen3 = src3.ptr<float>() +offset;
      auto down3 = cen3 - src3.cols;
      auto up3 = cen3 + src3.cols;

      float tmp0b = 0;
      float tmp1b = static_cast<float>(up1[0]) - static_cast<float>(down1[0]);
      float tmp2b = static_cast<float>(up1[1]) - static_cast<float>(down1[1]);
      float tmp0g = 0;
      float tmp1g = static_cast<float>(up2[0]) - static_cast<float>(down2[0]);
      float tmp2g = static_cast<float>(up2[1]) - static_cast<float>(down2[1]);
      float tmp0r = 0;
      float tmp1r = static_cast<float>(up3[0]) - static_cast<float>(down3[0]);
      float tmp2r = static_cast<float>(up3[1]) - static_cast<float>(down3[1]);
      ++offset;

      for (int x = 1; x < src1.cols - 1; x += 1, ++offset)
      {
        tmp0r = tmp1r;
        tmp0g = tmp1g;
        tmp0b = tmp1b;
        tmp1r = tmp2r;
        tmp1g = tmp2g;
        tmp1b = tmp2b;
        tmp2r = static_cast<float>(up3[x + 1]) - static_cast<float>(down3[x + 1]);
        tmp2g = static_cast<float>(up2[x + 1]) - static_cast<float>(down2[x + 1]);
        tmp2b = static_cast<float>(up1[x + 1]) - static_cast<float>(down1[x + 1]);

        ptrDx[offset] = maxAbsDiff(
          static_cast<float>(cen3[x + 1]) - static_cast<float>(cen3[x - 1])
          + static_cast<float>(down3[x + 1]) - static_cast<float>(down3[x - 1])
          + static_cast<float>(up3[x + 1]) - static_cast<float>(up3[x - 1]),
          static_cast<float>(cen2[x + 1]) - static_cast<float>(cen2[x - 1])
          + static_cast<float>(down2[x + 1]) - static_cast<float>(down2[x - 1])
          + static_cast<float>(up2[x + 1]) - static_cast<float>(up2[x - 1]),
          static_cast<float>(cen1[x + 1]) - static_cast<float>(cen1[x - 1])
          + static_cast<float>(down1[x + 1]) - static_cast<float>(down1[x - 1])
          + static_cast<float>(up1[x + 1]) - static_cast<float>(up1[x - 1]));
        ptrDy[offset] = maxAbsDiff(
          tmp0r + tmp1r + tmp2r,
          tmp0g + tmp1g + tmp2g,
          tmp0b + tmp1b + tmp2b);

        /*        ptrDy[offset] = maxAbsDiff(
        static_cast<float>(bgrUp[x - 1]) - static_cast<float>(bgrDown[x - 1])
        + static_cast<float>(bgrUp[x + 2]) - static_cast<float>(bgrDown[x + 2])
        + static_cast<float>(bgrUp[x + 5]) - static_cast<float>(bgrDown[x + 5]),
        static_cast<float>(bgrUp[x - 2]) - static_cast<float>(bgrDown[x - 2])
        + static_cast<float>(bgrUp[x + 1]) - static_cast<float>(bgrDown[x + 1])
        + static_cast<float>(bgrUp[x + 4]) - static_cast<float>(bgrDown[x + 4]),
        static_cast<float>(bgrUp[x - 3]) - static_cast<float>(bgrDown[x - 3])
        + static_cast<float>(bgrUp[x + 0]) - static_cast<float>(bgrDown[x + 0])
        + static_cast<float>(bgrUp[x + 3]) - static_cast<float>(bgrDown[x + 3]) );    */
      }
    }
  }
}
