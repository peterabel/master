#pragma once
#include <vector>
#include <opencv2/core/core.hpp>
#include "tracking/TrackingLogger.h"
#include "tracking/trackingMacros.h"
//////////////////////////////////////////////////////////////////////////////////////////////////////

class VCA_API Sample
{
public:
  //Sample( cv::Rect& roi, int ncols, float prob = 0.f);
  Sample(int x, int y, float scale, int ncols, float _probpos, float prob);
  Sample()
  {
    _x = 0;
    _y = 0;
    _scale = 0.f;
    /*  _roi.x = _roi.y = _roi.height = _roi.width = 0;*/
    _offset = 0;
    // _weight = 1.0f;
    _prob = 0.f;
    _probpos = 0.f;
  }

  //Sample( int x, int y, int width, int height, int ncols, float prob = 0.f);

public:
  // cv::Rect _roi;
  int _x;
  int _y;
  float _scale;
  // float _weight;
  int _offset;
  float _prob;
  float _probpos;
};


