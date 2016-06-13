#include <opencv2/imgproc/imgproc.hpp>
#include "features/Icf/IntegralFeatures.h"
#include "utils/Types/ArrUtils.h"
#include <chrono>


namespace vca
{
  /**
  * Constructor
  */

  icf::IntegralFeatures::IntegralFeatures( shared_ptr<Featurepipe> ft )
  {
    pipe = ft;
  }

  void icf::IntegralFeatures::update()
  {
    bool change = false;
    for( auto i = intImgs_.begin(); i != intImgs_.end(); ++i )
    {
      cv::Mat fimg;
      pipe->get( i->first, fimg );
      //+1 because integral image is one pixel larger in each dimension
      change = change | Vca::match(i->second, fimg.rows + 1, fimg.cols + 1, CV_64F);
    }
    if( change )
      updateFeatureChannelPointer();
    for( auto i = intImgs_.begin(); i != intImgs_.end(); ++i )
    {
      cv::Mat fimg;
      pipe->get( i->first, fimg );
      cv::integral( fimg, i->second, CV_64F );
    }    
  }

  void icf::IntegralFeatures::update( const cv::Mat& img )
   {
//	  auto t0 = std::chrono::high_resolution_clock::now();

    pipe->calcMap(img);
//	auto t1 = std::chrono::high_resolution_clock::now();
	//cout << "    feature channels: " << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count() << endl;
	update();
//	auto t2 = std::chrono::high_resolution_clock::now();
	//cout << "    integral images: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << endl;
  }


  void icf::IntegralFeatures::registerObserver( IntegralFeaturesObserver* observer )
  {
    registeredObserver.push_back( observer );
  }

  void icf::IntegralFeatures::deregisterObserver( IntegralFeaturesObserver* observer )
  {
    registeredObserver.erase(std::remove(registeredObserver.begin(), registeredObserver.end(), observer), registeredObserver.end());    
  }

  void icf::IntegralFeatures::getFeatureImg(vca::icf::ChannelType channel, cv::Mat& img) const
  {
    sassert( intImgs_.count(channel)>0);
    pipe->get( channel, img );
  }

  void icf::IntegralFeatures::getFeatureImgs(vector< const cv::Mat>& imgs) const
  {
    imgs.clear();
    for (auto it = intImgs_.begin(); it != intImgs_.end(); ++it )
    {
      cv::Mat featImg;
      getFeatureImg(  it->first,featImg );
      imgs.push_back( featImg );
    }
  }

  void icf::IntegralFeatures::activateChannel(ChannelType channel)
  {
    sassert(pipe->hasChannel(channel));
    if (intImgs_.count(channel) == 0)
    {
      intImgs_[channel] = cv::Mat();
      activeChannelTypes.push_back(channel);
      std::sort(activeChannelTypes.begin(), activeChannelTypes.end());
    }
  }

  void icf::IntegralFeatures::activateAllChannels()
  {
    auto ct = pipe->getChannelTypes();
    for (auto& c : ct)
    {
      activateChannel(c);
    }
  }

  void icf::IntegralFeatures::updateFeatureChannelPointer()
  {
    for( auto regIdx = 0; regIdx < registeredObserver.size(); ++regIdx )
    {
      registeredObserver[ regIdx ]->updateFeatureChannelPointer( intImgs_ );
    }
  }


}
