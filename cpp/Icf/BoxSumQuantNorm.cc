
#include "BoxSumQuantNorm.h"


namespace vca
{
  namespace icf
  {
    inline BoxSumQuantNorm::BoxSumQuantNorm() : upLeft_(nullptr), lowLeft_(nullptr)
    {

    }


    inline BoxSumQuantNorm::BoxSumQuantNorm(int x_, int y_, int w_, int h_, ChannelType channel_) : upLeft_(nullptr), lowLeft_(nullptr)
    {
      passert(channel_ == DIR_0 || channel_ == DIR_1 || channel_ == DIR_2 || channel_ == DIR_3 || channel_ == DIR_4 || channel_ == DIR_5);
      box = ChannelBox(x_, y_, w_, h_, channel_);
    }


    void BoxSumQuantNorm::activateChannels(shared_ptr<IntegralFeatures> feat) const
    {
      feat->activateChannel(box.channel);
      feat->activateChannel(MAG);
    }

    /*write to stream*/
    std::ostream& operator<<(std::ostream& out, const BoxSumQuantNorm& box)
    {
      out << box.box;
      return out;
    }


    /*read from stream*/
    istream& operator>>(istream& in, BoxSumQuantNorm& box)
    {
      in >> box.box;
      return in;
    }

    std::ostream& BoxSumQuantNorm::output(std::ostream& out) const
    {
      out << *this;
      return out;
    }

    std::istream& BoxSumQuantNorm::input(istream& in)
    {
      in >> *this;
      return in;
    }

    /**/
    inline float BoxSumQuantNorm::eval(void) const
    {
      return eval(0);
      sassert(upLeft_ != nullptr && lowLeft_ != nullptr);
      auto v1 = (*upLeft_ - upLeft_[box.w] - *lowLeft_ + lowLeft_[box.w]);
      auto v2 = (*upLeftNorm_ - upLeftNorm_[box.w] - *lowLeftNorm_ + lowLeftNorm_[box.w]);
      return static_cast<float>(v1 / v2);
    }


    /**/
    inline float BoxSumQuantNorm::eval(const uint off) const
    {
      sassert(upLeft_ != nullptr && lowLeft_ != nullptr);
      auto v1 = (upLeft_[off] - upLeft_[off + box.w] - lowLeft_[off] + lowLeft_[off + box.w]);
      auto v2 = (upLeftNorm_[off] - upLeftNorm_[off + box.w] - lowLeftNorm_[off] + lowLeftNorm_[off + box.w]);
      return static_cast<float>(v1 / v2);
    }


    /**/
    inline bool BoxSumQuantNorm::set(map< ChannelType, cv::Mat >& integralMap, int y, int x)
    {
      cv::Mat m = integralMap[box.channel];
      cv::Mat m_norm = integralMap[MAG];
      if (box.y + box.h + y + 1 >= (m.rows) || box.x + box.w + x + 1 >= (m.cols))
      {
        upLeft_ = nullptr;
        lowLeft_ = nullptr;
        upLeftNorm_ = nullptr;
        lowLeftNorm_ = nullptr;
        return false;
      }
      else
      {
        upLeft_ = m.row(box.y + y + 1).ptr< double >() + box.x + x + 1;
        lowLeft_ = m.row(box.y + box.h + y + 1).ptr< double >() + box.x + x + 1;
        upLeftNorm_ = m_norm.row(box.y + y + 1).ptr< double >() + box.x + x + 1;
        lowLeftNorm_ = m_norm.row(box.y + box.h + y + 1).ptr< double >() + box.x + x + 1;
        return true;

      }
    }


    /**/
    scalingFactors BoxSumQuantNorm::scaleFeatBox(IntegralFeatures& integralFeat, BoxSumQuantNorm& tar, const float scaleFactor, bool forceRoundDown /*= false */) const
    {
      tar = *this;
      scaleBox(tar.box, box, scaleFactor, forceRoundDown);

      tar.set(integralFeat.intImgs_, 0, 0);

    //  auto featureScale = integralFeat.pipe->getScalingFactor(tar.box.channel);
      //return powf(scaleFactor, featureScale);
      return{ 1.f, 1.f };

    }
  }
}

