#include "features/Icf/QuantDirDer.h"
#include "utils/Types/ArrUtils.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <chrono>
#include "convTri.h"

namespace vca
{

  /**
  * Default constructor
  * @param numQuants number of quantization steps
  */

  icf::QuantDirDer::QuantDirDer(int numNorm , int numQuants, bool softBin )
    : numQuants_(0), numNorm_(0)
  {

    cfg(numNorm, numQuants, softBin);


  }

  /**
  * Configure
  */

  void icf::QuantDirDer::cfg(int numNorm, int numQuants, bool softBin)
  {
    FEATURES_DEBUG( "Quantization levels " << numQuants );
    passert( numQuants > 0 );
    softbin_ = softBin;
    numNorm_ = numNorm;
    numQuants_ = numQuants;
    binWidth_ = ( float( M_PI ) + 1.0e-5f ) / float( numQuants_ );
    eps_ = 1.0e-10f;

    dirQuants.resize( numQuants_ );

    //if( numQuants > 0 )
    //  channelType2FeatureChannel[DIR_0]=dirQuants[0];
    //if( numQuants > 1 )
    //  channelType2FeatureChannel[DIR_1]=dirQuants[1];
    //if( numQuants > 2 )
    //  channelType2FeatureChannel[DIR_2]=dirQuants[2];
    //if( numQuants > 3 )
    //  channelType2FeatureChannel[DIR_3]=dirQuants[3];
    //if( numQuants > 4 )
    //  channelType2FeatureChannel[DIR_4]=dirQuants[4];
    //if( numQuants > 5 )
    //  channelType2FeatureChannel[DIR_5]=dirQuants[5];
    //channelType2FeatureChannel[MAG]=mag;
  }

  /**
  *
  */




  void icf::QuantDirDer::GradientMagnitude( const cv::Mat& dx, const cv::Mat& dy )
  {

    sassert( dx.isContinuous() && dy.isContinuous() && Vca::hasDim( dx,dy ) );
    sassert( dx.type() == CV_32FC1 && dy.type() == CV_32FC1 );

    Vca::match( dx, mag, CV_32FC1 );

    float* tarPtr = mag.ptr<float>();
    cv::Mat tmp(mag.size(), CV_32FC1);
    const float *dxPtr = dx.ptr< float >();
    const float *dyPtr = dy.ptr< float >();


 //   #pragma omp parallel for schedule(dynamic)
    for( auto idx = 0; idx < dx.rows * dx.cols; ++idx )
    {
      tarPtr[ idx ] = sqrtf( (float)(dxPtr[ idx ] * dxPtr[ idx ] 
                                  + dyPtr[ idx ] * dyPtr[ idx ]) );
    }
    if (numNorm_ >0)
    {
      convTri::convTri(tarPtr, tmp.ptr<float>() , mag.cols, mag.rows, (numNorm_ - 1) / 2);
     // mag = tmp;
      mag = mag/(tmp+0.005);
      //cv::Mat tmpBoxed;
      //cv::boxFilter(mag, tmpBoxed, -1, cv::Size(numNorm_, numNorm_), cv::Point(-1, -1), false);
      //mag = mag / tmpBoxed;
    }
  } 


  void icf::QuantDirDer::GradientMagnitude(const cv::Mat& dxp, const cv::Mat& dxn, const cv::Mat& dyp, const cv::Mat& dyn)
  {
    sassert( dxp.isContinuous() && dyp.isContinuous() 
      && dxn.isContinuous() && dyn.isContinuous() 
      && Vca::hasDim( dxp,dxn )&& Vca::hasDim( dxp,dyp )&& Vca::hasDim( dxp,dyn ) );
    sassert( dxp.type() == CV_32FC1 && dxn.type() == CV_32FC1 && dyp.type() == CV_32FC1 && dyn.type() == CV_32FC1 );


    Vca::match( dxp, mag, CV_32FC1 );

    float* tarPtr = mag.ptr<float>();
    const float *dxpPtr = dxp.ptr< float >();
    const float *dxnPtr = dxn.ptr< float >();
    const float *dypPtr = dyp.ptr< float >();
    const float *dynPtr = dyn.ptr< float >();


    //   #pragma omp parallel for schedule(dynamic)
    for( auto idx = 0; idx < dxp.rows * dxp.cols; ++idx )
    {
      tarPtr[ idx ] = sqrtf( (float)((dxpPtr[ idx ] - dxnPtr[ idx ] )* (dxpPtr[ idx ] - dxnPtr[ idx ] )
      + (dypPtr[ idx ] - dynPtr[ idx ] ) * (dypPtr[ idx ] - dynPtr[ idx ] ) ));
    }
    cv::Mat tmp(mag.size(), CV_32FC1);

    if (numNorm_ >0)
    {
      convTri::convTri(tarPtr, tmp.ptr<float>(), mag.cols, mag.rows, (numNorm_ - 1) / 2);
      // mag = tmp;
      mag = mag / (tmp + 0.005);
    }
  }


  void icf::QuantDirDer::GradientDirection(const cv::Mat& dxp, const cv::Mat& dxn, const cv::Mat& dyp, const cv::Mat& dyn, float eps)
  {

    sassert( dxp.isContinuous() && dyp.isContinuous() 
      && dxn.isContinuous() && dyn.isContinuous() 
      && Vca::hasDim( dxp,dxn )&& Vca::hasDim( dxp,dyp )&& Vca::hasDim( dxp,dyn ) );
    sassert( dxp.type() == CV_32FC1 && dxn.type() == CV_32FC1 && dyp.type() == CV_32FC1 && dyn.type() == CV_32FC1 );
    sassert( dirQuants.size() == numQuants_ );


    const float* magPtr = mag.ptr<float>();
    const float *dxpPtr = dxp.ptr< float >();
    const float *dxnPtr = dxn.ptr< float >();
    const float *dypPtr = dyp.ptr< float >();
    const float *dynPtr = dyn.ptr< float >();
    Vca::match( dxp, dirQuants, CV_32FC1 );

    mapPtr_.resize( numQuants_, nullptr );
    for( auto idx=0; idx < numQuants_; idx ++ )
    {
      dirQuants[ idx].setTo( 0.f);
      mapPtr_[ idx ] = dirQuants[idx].ptr<float>();
    }

    #pragma omp parallel for
    for( auto idx = 0; idx < dxp.rows * dxp.cols; idx ++ )
    {
      if( magPtr[ idx ] > eps )
      {
        /* calculate the angle from the gradient in x and y direction, 
           resulting ang is in the interval [-pi,+pi] */
        auto ang = atan2f( dypPtr[ idx ] - dynPtr[ idx ], dxpPtr[ idx ] - dxnPtr[ idx ] );
        /* add half bin width so that the bin gradient direction is in the center of the bin */
        ang += binWidth_ * 0.5f;
        /* angles that are directed opposite are quantized in the same bin, therefore PI is added
          so that the angle now in the interval [0,+pi +gs_PD_BinWidthHalf ]  */
        if( ang < 0.0f )
          ang += (float)M_PI;
        if( ang >M_PI )
          ang -= (float)M_PI;
    
        /*calculate the index of the to the angle	corresponding gradient direction feature channel
          and sets the pixel to the gradient magnitude */
        mapPtr_[(int)( ang / binWidth_ ) ][ idx ] = getQuantValue(magPtr[ idx ]);
      }
    }
  }

  void icf::QuantDirDer::GradientDirection( const cv::Mat& dx, const cv::Mat& dy, float eps )
  {
    sassert( dx.isContinuous() && dy.isContinuous() && Vca::hasDim( dx,dy ) );
    sassert( dx.type() == CV_32FC1 && dy.type() == CV_32FC1 );
    sassert( dirQuants.size() == numQuants_ );

    const float* magPtr = mag.ptr<float>();
    const float *dxPtr = dx.ptr< float >();
    const float *dyPtr = dy.ptr< float >();
    Vca::match( dx, dirQuants, CV_32FC1 );

    mapPtr_.resize( numQuants_, nullptr );
    for( auto idx=0; idx < numQuants_; idx ++ )
    {
      dirQuants[ idx].setTo( 0.f);
      mapPtr_[ idx ] = dirQuants[idx].ptr<float>();
    }
	float bhalf = binWidth_ * 0.5f;

    #pragma omp parallel for
    for( auto idx = 0; idx < dx.rows * dx.cols; idx ++ )
    {
      if( magPtr[ idx ] > eps )
      {
        /* calculate the angle from the gradient in x and y direction, 
           resulting ang is in the interval [-pi,+pi] */
		  auto ang = atan2f(static_cast<float>(dyPtr[idx]), static_cast<float>(dxPtr[idx]));


        /*calculate the index of the to the angle	corresponding gradient direction feature channel
          and sets the pixel to the gradient magnitude */

        if (!softbin_)
        {
          /* add half bin width so that the bin gradient direction is in the center of the bin */
          ang += bhalf;
          /* angles that are directed opposite are quantized in the same bin, therefore PI is added
          so that the angle now in the interval [0,+pi +gs_PD_BinWidthHalf ]  */
          if (ang < 0.0f)
            ang += static_cast<float>(M_PI);
          if (ang >M_PI)
            ang -= static_cast<float>(M_PI);
          auto idxSoft = ang / binWidth_;
          int idx1 = static_cast<int>(ang / binWidth_);
          mapPtr_[idx1][idx] = getQuantValue(magPtr[idx]);

        }
        else
        {
          /* angles that are directed opposite are quantized in the same bin, therefore PI is added
          so that the angle now in the interval [0,+pi +gs_PD_BinWidthHalf ]  */
          if (ang < 0.0f)
            ang += static_cast<float>(M_PI);
          if (ang >M_PI)
            ang -= static_cast<float>(M_PI);
          auto idxSoft = ang / binWidth_;
          int idx1 = static_cast<int>(ang / binWidth_);
          sassert((1. - (idxSoft - idx1)) >= 0)
          sassert((1. - (idxSoft - idx1)) <= 1)
          mapPtr_[idx1][idx] = (1.f - idxSoft + idx1 ) *getQuantValue(magPtr[idx]);
          mapPtr_[(idx1 + 1) % numQuants_][idx] = (idxSoft - idx1)* getQuantValue(magPtr[idx]);
        }
      }
    }
  } 
  void icf::QuantDirDer::calcMap( const cv::Mat& dx, const cv::Mat& dy )
  {
    sassert( dx.isContinuous() && dy.isContinuous() && Vca::hasDim( dx,dy ) );
    sassert( dx.type() == CV_32FC1 && dy.type() == CV_32FC1 );
    sassert( dirQuants.size() == numQuants_ );

	//auto t0_grad = std::chrono::high_resolution_clock::now();

    GradientMagnitude( dx, dy );
	//auto t1_grad = std::chrono::high_resolution_clock::now();
	//cout << "        mag " << std::chrono::duration_cast<std::chrono::milliseconds>(t1_grad - t0_grad).count() << endl;
	GradientDirection( dx, dy, eps_ );
	//auto t2_grad = std::chrono::high_resolution_clock::now();
	//cout << "        quant: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2_grad - t1_grad).count() << endl;
  }

  void icf::QuantDirDer::calcMap(const cv::Mat& dxp, const cv::Mat& dxn, const cv::Mat& dyp, const cv::Mat& dyn)
  {
    sassert( dxp.isContinuous() && dyp.isContinuous() 
      && dxn.isContinuous() && dyn.isContinuous() 
      && Vca::hasDim( dxp,dxn )&& Vca::hasDim( dxp,dyp )&& Vca::hasDim( dxp,dyn ) );
    sassert( dxp.type() == CV_32FC1 && dxn.type() == CV_32FC1 && dyp.type() == CV_32FC1 && dyn.type() == CV_32FC1 );
    sassert( dirQuants.size() == numQuants_ );

    GradientMagnitude( dxp, dxn, dyp, dyn );
    GradientDirection( dxp, dxn, dyp, dyn, eps_ );
  }

}
