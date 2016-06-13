#include "features/Icf/Magnitude.h"
#include "utils/Types/ArrUtils.h"

namespace vca
{

  void icf::Magnitude::calcMap( const cv::Mat& dx, const cv::Mat& dy )
  {
    sassert( dx.isContinuous() && dy.isContinuous() && Vca::hasDim( dx,dy ) );
    sassert( dx.type() == CV_32FC1 && dy.type() == CV_32FC1 ); 

    Vca::match( dx, mag, CV_32FC1 );

    float* tarPtr = mag.ptr<float>();
    const float *dxPtr = dx.ptr< float >();
    const float *dyPtr = dy.ptr< float >();


    //   #pragma omp parallel for schedule(dynamic)
    for( auto idx = 0; idx < dx.rows * dx.cols; ++idx )
    {
      tarPtr[ idx ] = sqrtf( static_cast<float>(dxPtr[ idx ] * dxPtr[ idx ] 
      + dyPtr[ idx ] * dyPtr[ idx ]) );
    }
  }
  void icf::Magnitude::calcMap(const cv::Mat& dxp, const cv::Mat& dxn, const cv::Mat& dyp, const cv::Mat& dyn)
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
  }
}
