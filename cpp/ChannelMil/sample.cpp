#include "sample.h"
#include "milutil.h"


Sample::Sample(int x, int y, float scale, int ncols, float probpos, float prob)
{
  _x = x;
  _y = y;
  _scale = scale;

  // +1 because integral images is 1 pixel larger than the original image
  _offset = (ncols + 1) * _y + _x;
  _prob = prob;
  _probpos = probpos;
}
