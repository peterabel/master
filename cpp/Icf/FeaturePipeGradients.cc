#include "features/Icf/FeaturePipeGradients.h"

//namespace Vca
//{
//  /**
//  *
//  */
//
//  //Icf::FeaturePipeGradient::FeaturePipeGradient()
//  //{
//  //  quantDir_.cfg( nGradientDir );
//  //  height_ = width_ = 0;
//  //  border_ = 0;
//  //}
//
//
//  //void  Icf::FeaturePipeGradient::get( int idx, cv::Mat& img )
//  //{
//  //  sassert( idx >= 0 && idx < size() );
//  //  if( idx == nGradientDir )
//  //    img =  quantDir_.mag;
//  //  else
//  //    img =  quantDir_.dirQuants[ idx ];
//  //}
//
//  ///**
//  //*
//  //*/
//
//  //void Icf::FeaturePipeGradient::calcMap( const cv::Mat& img )
//  //{
//  //  cv::Mat tmp = img;
//  //  //cout << img.rows << " " << img.cols << " " << border_ << endl;
//  //  if( border_ > 0 )
//  //    cv::copyMakeBorder( img, tmp, border_, border_, border_, border_, cv::BORDER_REPLICATE );
//  //  height_ = tmp.rows;
//  //  width_ = tmp.cols;
//  //
//  //  maxPar_.calcMap( tmp );
//  //  quantDir_.calcMap( maxPar_.dx, maxPar_.dy );
//  //       
//  }
//}
