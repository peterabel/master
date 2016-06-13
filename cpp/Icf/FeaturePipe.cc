//#include "features/Icf/FeaturePipe.h"
//#include "FeaturePipeGradientsLuv.h"
//#include "FeaturePipeGradients.h"
//
//namespace Vca
//{
//  namespace Icf
//  {
//    height_ = width_ = 0;
//    border_ = 0;
//  }
//
//      if( useLuv_ )
//      {
//        luv_ = new LuvMaps();
//        channels_.insert( pair< ChannelType, cv::Mat >( LUV_L, luv_->lMap ) );
//        channels_.insert( pair< ChannelType, cv::Mat >( LUV_U, luv_->uMap ) );
//        channels_.insert( pair< ChannelType, cv::Mat >( LUV_V, luv_->vMap ) );
//      }
//      if( useQuantDir_ )
//      {
//        quantDir_ = new QuantDirDer();
//        quantDir_->cfg(6);
//        channels_.insert( pair< ChannelType, cv::Mat >( DIR_0, quantDir_->dirQuants[ 0 ] ) );
//        channels_.insert( pair< ChannelType, cv::Mat >( DIR_1, quantDir_->dirQuants[ 1 ] ) );
//        channels_.insert( pair< ChannelType, cv::Mat >( DIR_2, quantDir_->dirQuants[ 2 ] ) );
//        channels_.insert( pair< ChannelType, cv::Mat >( DIR_3, quantDir_->dirQuants[ 3 ] ) );
//        channels_.insert( pair< ChannelType, cv::Mat >( DIR_4, quantDir_->dirQuants[ 4 ] ) );
//        channels_.insert( pair< ChannelType, cv::Mat >( DIR_5, quantDir_->dirQuants[ 5 ] ) );
//        channels_.insert( pair< ChannelType, cv::Mat >( MAG, quantDir_->mag ) );
//
//      }
//      if( useMaxPar_ )
//      {
//        maxPar_ = new MaxParDer();
//        channels_.insert( pair< ChannelType, cv::Mat >( D_X, maxPar_->dx ) );
//        channels_.insert( pair< ChannelType, cv::Mat >( D_Y, maxPar_->dy ) );
//      }
// 
//      height_ = width_ = 0;
//    }
//
//    /** 
//    * destructor
//    */
//    FeaturePipe::~FeaturePipe()
//    {
//      if( useLuv_ )
//      {
//        delete luv_;
//      }
//      if( useMaxPar_ )
//      {
//        delete maxPar_;
//      }
//      if( useQuantDir_ )
//      {
//        delete quantDir_;
//      }
//    }
//
//    /** 
//    * 
//    */
//    void FeaturePipe::calcMap( const cv::Mat& src )
//    {
//      height_ = src.rows;
//      width_ = src.cols;
//      if( useLuv_ )
//      {
//        luv_->calcMap( src );
//        channels_[ LUV_L ] = luv_->lMap;
//        channels_[ LUV_U ] = luv_->uMap;
//        channels_[ LUV_V ] = luv_->vMap;
//      }
//      if( useMaxPar_ )
//      {
//        maxPar_->calcMap( src );
//        (maxPar_->dx).convertTo( channels_[ D_X ], CV_32F );
//        (maxPar_->dy).convertTo( channels_[ D_Y ], CV_32F );
//      }
//      if( useQuantDir_ )
//      {
//        sassert( useMaxPar_ );
//        quantDir_->calcMap( maxPar_->dx, maxPar_->dy );
//        channels_[ DIR_0 ] = quantDir_->dirQuants[ 0 ];
//        channels_[ DIR_1 ] = quantDir_->dirQuants[ 1 ];
//        channels_[ DIR_2 ] = quantDir_->dirQuants[ 2 ];
//        channels_[ DIR_3 ] = quantDir_->dirQuants[ 3 ];
//        channels_[ DIR_4 ] = quantDir_->dirQuants[ 4 ];
//        channels_[ DIR_5 ] = quantDir_->dirQuants[ 5 ];
//        channels_[ MAG ] = quantDir_->mag;
//      }
//    }
//
//    /** 
//    * 
//    */
//    void FeaturePipe::get( ChannelType channel, cv::Mat& img )
//    {
//      auto iter = channels_.find( channel );
//      if( iter != channels_.end() )
//      {
//        img = iter->second;
//      }
//      else
//      {
//        cout << "Channel does not exist!" << endl;
//      }
//    }
//
//    map< ChannelType, cv::Mat > FeaturePipe::getCopy() const
//    {
//      map< ChannelType, cv::Mat > result;
//      cv::Mat img;
//      for( auto i = channels_.begin(); i != channels_.end(); i++ )
//      {
//        i->second.copyTo(img);
//        result.insert( pair<ChannelType, cv::Mat>(i->first, img ) );
//      }
//      return result;
//    }
//
//    /** 
//    * 
//    */
//    int FeaturePipe::size()
//    {
//      int nMaps = 0;
//      if( useLuv_ )
//      {
//        nMaps += 3;
//      }
//      if( useMaxPar_ )
//      {
//        nMaps += 2;
//      }
//      if( useQuantDir_ )
//      {
//        nMaps += 7;
//      }
//      
//      return nMaps;
//    }
//
//
//    /**/
//    FeaturePipe::channelStringConverter::channelStringConverter()
//    {
//      c2s_.insert(pair<ChannelType,string>(LUV_L, "LUV_L"));
//      c2s_.insert(pair<ChannelType,string>(LUV_U, "LUV_U"));
//      c2s_.insert(pair<ChannelType,string>(LUV_V, "LUV_V"));
//      c2s_.insert(pair<ChannelType,string>(LUV_V, "LUV_V"));
//      c2s_.insert(pair<ChannelType,string>(DIR_0, "DIR_0"));
//      c2s_.insert(pair<ChannelType,string>(DIR_1, "DIR_1"));
//      c2s_.insert(pair<ChannelType,string>(DIR_2, "DIR_2"));
//      c2s_.insert(pair<ChannelType,string>(DIR_3, "DIR_3"));
//      c2s_.insert(pair<ChannelType,string>(DIR_4, "DIR_4"));
//      c2s_.insert(pair<ChannelType,string>(DIR_5, "DIR_5"));
//      c2s_.insert(pair<ChannelType,string>(MAG, "MAG"));
//      c2s_.insert(pair<ChannelType,string>(D_X, "D_X"));
//      c2s_.insert(pair<ChannelType,string>(D_Y, "D_Y"));
//      s2c_.insert(pair<string,ChannelType>("LUV_L", LUV_L));
//      s2c_.insert(pair<string,ChannelType>("LUV_U", LUV_U));
//      s2c_.insert(pair<string,ChannelType>("LUV_V", LUV_V));
//      s2c_.insert(pair<string,ChannelType>("DIR_0", DIR_0));
//      s2c_.insert(pair<string,ChannelType>("DIR_1", DIR_1));
//      s2c_.insert(pair<string,ChannelType>("DIR_2", DIR_2));
//      s2c_.insert(pair<string,ChannelType>("DIR_3", DIR_3));
//      s2c_.insert(pair<string,ChannelType>("DIR_4", DIR_4));
//      s2c_.insert(pair<string,ChannelType>("DIR_5", DIR_5));
//      s2c_.insert(pair<string,ChannelType>("MAG", MAG));
//      s2c_.insert(pair<string,ChannelType>("D_X", D_X));
//      s2c_.insert(pair<string,ChannelType>("D_Y", D_Y));
//    }
//
//
//    /**/
//    string FeaturePipe::channelStringConverter::channel2string( ChannelType c ) const
//    {
//      string result = "";
//      map< ChannelType, string >::const_iterator iter = c2s_.find( c );
//      if( iter != c2s_.end() )
//        result = iter->second;
//
//      return result;
//    }
//
//
//    /**/
//    ChannelType FeaturePipe::channelStringConverter::string2channel( string s ) const
//    {
//      ChannelType result;
//      map< string, ChannelType >::const_iterator iter = s2c_.find( s );
//      if( iter != s2c_.end() )
//        result = iter->second;
//
//      return result;
//    }
//  }
//}
