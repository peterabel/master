#include "features/Icf/FeaturePipeGradientsLuv.h"
//
//namespace Vca
//{
//  /**
//  *
//  */
//
//  Icf::FeaturePipeGradientsLuv::FeaturePipeGradientsLuv()
//  {
//    quantDir_.cfg( 6 );
//    height_ = width_ = 0;
//    border_ = 0;
//  }
//
//
//  void  Icf::FeaturePipeGradientsLuv::get( int idx, cv::Mat& img )
//  {
//    sassert( idx >= 0 && idx < size() );
//    if( idx == 0 )
//      img = luv_.lMap;
//    else if( idx == 1 )
//      img =  luv_.uMap;
//    else if( idx == 2 )
//      img =  luv_.vMap;
//    else if( idx == 9 )
//      img =  quantDir_.mag;
//    else
//      img =  quantDir_.dirQuants[ idx - 3];
//  }
//
//  /**
//  *
//  */
//
//  void Icf::FeaturePipeGradientsLuv::calcMap( const cv::Mat& img )
//  {
//    cv::Mat tmp = img;
//    //cout << img.rows << " " << img.cols << " " << border_ << endl;
//    if( border_ > 0 )
//      cv::copyMakeBorder( img, tmp, border_, border_, border_, border_, cv::BORDER_REPLICATE );
//    height_ = tmp.rows;
//    width_ = tmp.cols;
//    #pragma omp parallel sections
//    {
//      #pragma omp section
//      {
//        maxPar_.calcMap( tmp );
//        quantDir_.calcMap( maxPar_.dx, maxPar_.dy );
//      }
//
//       #pragma omp section
//      {
//        luv_.calcMap( tmp );    
//      }
//    }    
//  }
//}
