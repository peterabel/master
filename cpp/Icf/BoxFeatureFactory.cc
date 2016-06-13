#include "BoxFeatureFactory.h"
#include "BoxDxDyDxyDyx.h"



vca::icf::BoxFeatureFactory::BoxFeatureFactory(  BoxFeatureConfig& boxFeatCofig)
{
  boxConf = boxFeatCofig;

  if( boxConf.useLuv && boxConf.useGray )
  {
    //TODO error/information messages
    boxConf.useGray = false;
  }
  if (boxConf.useLuv && boxConf.useUV)
  {
    //TODO error/information messages
    boxConf.useUV = false;
  }

  if( boxConf.useDxDyDxyDyx)
  {
    boxConf.useLuv = true;
    boxConf.useGray = false;
    boxConf.useQuanDxDy = false;
    boxConf.useDxDy = false;
    boxConf.useDxDyMag = false;
    boxConf.useAbsDxDy = false;
    boxConf.useAbsDxDyMag = false;
    boxConf.useQuanAbsDxDy = false;
    boxConf.useQuanDir = false;
    boxConf.useQuanDirNorm = false;
  }

  sumBoxWeight = 1;
  sumBoxNormWeight = 0;
  dxdyBoxWeight = 0;
  absdxdyBoxWeight = 0;
  quandxdyBoxWeight = 0;
  dxdydxydyxBoxWeight = 0;
  usedChannelTypes.push_back( MAG);
  
  if(boxConf.useLuv )
  {
   usedChannelTypes.push_back( LUV_L);
   usedChannelTypes.push_back( LUV_U);
   usedChannelTypes.push_back( LUV_V);
    sumBoxWeight += 3;
  }
  if (boxConf.useUV)
  {
  //  usedChannelTypes.push_back(LUV_L);
    usedChannelTypes.push_back(LUV_U);
    usedChannelTypes.push_back(LUV_V);
    sumBoxWeight += 2;
  }

  if (boxConf.useGray)
  {
   usedChannelTypes.push_back( GRAY );
    sumBoxWeight += 1;
  }
  if (boxConf.useQuanDir || boxConf.useQuanDirNorm || boxConf.useQuanDirNorm2)
  {
   usedChannelTypes.push_back( DIR_0);
   usedChannelTypes.push_back( DIR_1);
   usedChannelTypes.push_back( DIR_2);
   usedChannelTypes.push_back( DIR_3);
   usedChannelTypes.push_back( DIR_4);
   usedChannelTypes.push_back( DIR_5);
   if (boxConf.useQuanDir || boxConf.useQuanDirNorm)
     sumBoxWeight += 6;
   else
     sumBoxNormWeight += 6;
  }
  if (boxConf.useDxDy || boxConf.useDxDyMag)
  {
    dxdyBoxWeight += 6;
  }
  if (boxConf.useAbsDxDy || boxConf.useAbsDxDyMag)
  {
    absdxdyBoxWeight += 6;
  }
  if (boxConf.useQuanAbsDxDy || boxConf.useQuanDxDy)
  {
    quandxdyBoxWeight += 6;
  }
  if( boxConf.useDxDyDxyDyx )
  {
    dxdydxydyxBoxWeight += 6;
  }
}

void vca::icf::BoxFeatureFactory::generateFeatures(FeatureContainer& featcont, int n, int minX, int minY, int maxX, int maxY,
  int minWidth, int minHeight, int maxWidth, int maxHeight, default_random_engine& randomEng)
{
  if (boxConf.useGridFeatures)
  {
    generateFeaturesGrid(featcont, n, minX, minY, maxX, maxY,  0.5f);
  }
  else
  {
    generateFeaturesRandom(featcont, n, minX, minY, maxX, maxY,
      minWidth, minHeight, maxWidth, maxHeight, randomEng);
  }

}


void vca::icf::BoxFeatureFactory::generateFeaturesRandom2(FeatureContainer& featcont, int n, int step, int minX, int minY, int maxX, int maxY,
  int minWidth, int minHeight, int maxWidth, int maxHeight, default_random_engine& randomEng)
{
  maxWidth = min(maxWidth, maxX - minX);
  maxHeight = min(maxHeight, maxY - minY);
  if (boxConf.useOnlySquare)
  {
    minWidth = max(minWidth, minHeight);
    maxWidth = min(maxWidth, maxHeight);
    minHeight = minWidth;
    maxHeight = maxWidth;
  }
  sassert(minHeight <= maxHeight);
  sassert(minWidth <= maxWidth);
  float sumWeight = static_cast<float>(dxdyBoxWeight + sumBoxWeight + absdxdyBoxWeight + quandxdyBoxWeight + dxdydxydyxBoxWeight + sumBoxNormWeight);
  std::uniform_real_distribution<float> typeDist(0.f, sumWeight);
  std::uniform_real_distribution<float> dist01(0.f, 1.f);
  std::uniform_real_distribution<float> directionDist(0.f, static_cast<float>(M_PI));
  std::uniform_int_distribution<int> boxWidthDist(minWidth, maxWidth);
  std::uniform_int_distribution<int> boxHeightDist(minHeight, maxHeight);
  std::uniform_int_distribution<int> channelDist(0, usedChannelTypes.size() - 1);
  std::uniform_int_distribution<int> channelDistNorm(0, max(sumBoxNormWeight - 1, 1));
  std::uniform_int_distribution<int> diceDist(0, 1);

  vector<int> cumNXPos;
  int sum = 0;
  for (int w = minWidth; w < maxWidth; w += step)
  {
    int nPos = (maxX - minX - w)/step + 1;
    sum += nPos;
    cumNXPos.push_back(sum);
  }
  vector<int> cumNYPos;
  vector<int> cumNYXPos;
  sum = 0;
  int sumXY = 0;
  for (int h = minHeight; h < maxHeight; h+= step)
  {
    int nPos = (maxY - minY - h)/step + 1;
    sum += nPos;
    cumNYPos.push_back(sum);

    int nPosX = (maxX - minX - h)/step + 1;
    sumXY += nPos * nPosX;
    cumNYXPos.push_back(sumXY);
  }


  for (auto i = 0; i < n; ++i)
  {
    int width = 0;
    int height = 0;
    if (boxConf.useOnlySquare)
    {
      float v = dist01(randomEng);
      width = height = minWidth + getRandomCumIdx(v, cumNYXPos)*step;
    }
    else
    {
      float v = dist01(randomEng);
      height = minHeight + getRandomCumIdx(v, cumNYPos)*step;
      v = dist01(randomEng);
      width = minWidth + getRandomCumIdx(v, cumNXPos)*step;
      //boxHeightDist( randomEng );
      //boxWidthDist( randomEng );
    }
    float xRel = dist01(randomEng);
    float yRel = dist01(randomEng);

    int x = static_cast<int>((maxX - width - minX)*xRel/step + .5f)*step + minX;
    int y = static_cast<int>((maxY - height - minY)*yRel / step + .5f)*step + minY;

    sassert(x >= minX);
    sassert(y >= minY);
    sassert(x + width <= maxX);
    sassert(y + height <= maxY);

    float boxTypeRandom = typeDist(randomEng);
    float dir = 0.f;
    if (boxTypeRandom < dxdyBoxWeight)
    {
      sassert(boxConf.useDxDy || boxConf.useDxDyMag);
      dir = directionDist(randomEng);
      bool useMag = false;
      if (boxConf.useDxDy && boxConf.useDxDyMag)
      {
        useMag = (diceDist(randomEng) > 0);
      }
      else if (boxConf.useDxDyMag) useMag = true;
      if (!boxConf.useAbsDxDy && !boxConf.useAbsDxDyMag &&!boxConf.useQuanAbsDxDy && !boxConf.useQuanDxDy)
        featcont.addDxdyBoxFeature(x, y, width, height, dir, useMag);
      else
        featcont.addAbsDxdyBoxFeature(x, y, width, height, dir, useMag, false);

    }
    else if (boxTypeRandom < dxdyBoxWeight + absdxdyBoxWeight)
    {
      sassert(boxConf.useAbsDxDy || boxConf.useAbsDxDyMag);
      dir = directionDist(randomEng);
      bool useMag = false;
      if (boxConf.useAbsDxDy && boxConf.useAbsDxDyMag)
      {
        useMag = (diceDist(randomEng) > 0);
      }
      else if (boxConf.useAbsDxDyMag) useMag = true;

      featcont.addAbsDxdyBoxFeature(x, y, width, height, dir, useMag, true);
    }
    else if (boxTypeRandom < dxdyBoxWeight + absdxdyBoxWeight + quandxdyBoxWeight)
    {
      sassert(boxConf.useQuanAbsDxDy || boxConf.useQuanDxDy);
      float minDir = directionDist(randomEng);
      float maxDir = directionDist(randomEng);
      bool useAbs = false;
      if (boxConf.useQuanAbsDxDy && boxConf.useQuanDxDy)
      {
        useAbs = (diceDist(randomEng) > 0);
      }
      else if (boxConf.useQuanAbsDxDy) useAbs = true;

      featcont.addQuanBoxFeature(x, y, width, height, minDir, maxDir, useAbs);
    }
    else if (boxTypeRandom < dxdyBoxWeight + absdxdyBoxWeight + quandxdyBoxWeight + dxdydxydyxBoxWeight)
    {
      dir = directionDist(randomEng)*.5f;
      featcont.addBoxFeature(BoxDxDyDxyDyx(x, y, width, height, D_X, dir, true));
    }
    else if (boxTypeRandom < dxdyBoxWeight + absdxdyBoxWeight + quandxdyBoxWeight + dxdydxydyxBoxWeight + sumBoxNormWeight)
    {
      int channelIdx = channelDistNorm(randomEng);
      icf::ChannelType c = DIR_0;
      if (channelIdx == 1)
        c = DIR_1;
      if (channelIdx == 2)
        c = DIR_2;
      if (channelIdx == 3)
        c = DIR_3;
      if (channelIdx == 4)
        c = DIR_4;
      if (channelIdx == 5)
        c = DIR_5;

      featcont.addSumBoxNormFeature(x, y, width, height, c);
    }
    else
    {
      int channelIdx = channelDist(randomEng);
      featcont.addSumBoxFeature(x, y, width, height, usedChannelTypes[channelIdx]);
    }
  }
}

void vca::icf::BoxFeatureFactory::generateFeaturesRandom(FeatureContainer& featcont, int n,  int minX, int minY, int maxX, int maxY,
                                                  int minWidth, int minHeight, int maxWidth, int maxHeight, default_random_engine& randomEng)
{
  maxWidth = min( maxWidth, maxX - minX );
  maxHeight = min( maxHeight, maxY - minY);
  if( boxConf.useOnlySquare )
  {
    minWidth = max( minWidth, minHeight );
    maxWidth = min( maxWidth, maxHeight );
    minHeight = minWidth;
    maxHeight = maxWidth;
  } 
  sassert( minHeight <= maxHeight);
  sassert( minWidth <= maxWidth );
  float sumWeight = static_cast<float>(dxdyBoxWeight+sumBoxWeight+absdxdyBoxWeight+quandxdyBoxWeight+dxdydxydyxBoxWeight+sumBoxNormWeight);
  std::uniform_real_distribution<float> typeDist(0.f,sumWeight);
  std::uniform_real_distribution<float> dist01(0.f,1.f);
  std::uniform_real_distribution<float> directionDist(0.f,static_cast<float>(M_PI));
  std::uniform_int_distribution<int> boxWidthDist( minWidth, maxWidth );
  std::uniform_int_distribution<int> boxHeightDist( minHeight, maxHeight );
  std::uniform_int_distribution<int> channelDist(0, usedChannelTypes.size() - 1);
  std::uniform_int_distribution<int> channelDistNorm(0, max(sumBoxNormWeight - 1,1));
  std::uniform_int_distribution<int> diceDist( 0, 1 );

  vector<int> cumNXPos;
  int sum = 0;
  for( int w = minWidth; w < maxWidth; ++w)
  {
    int nPos = maxX - minX - w + 1;
    sum += nPos;
    cumNXPos.push_back( sum );
  }
  vector<int> cumNYPos;
  vector<int> cumNYXPos;
  sum = 0;
  int sumXY = 0;
  for( int h = minHeight; h < maxHeight; ++h)
  {
    int nPos = maxY - minY - h + 1;
    sum += nPos;
    cumNYPos.push_back( sum );

    int nPosX = maxX - minX - h + 1;
    sumXY += nPos * nPosX;
    cumNYXPos.push_back( sumXY );
  }


  for (auto i = 0; i < n; ++i )
  {
    int width = 0;
    int height = 0;
    if( boxConf.useOnlySquare )
    {
      float v = dist01( randomEng );
      width = height = minWidth + getRandomCumIdx( v, cumNYXPos );
    }
    else
    {
      float v = dist01( randomEng );
      height = minHeight + getRandomCumIdx( v, cumNYPos );
      v = dist01( randomEng );
      width = minWidth + getRandomCumIdx( v, cumNXPos );
      //boxHeightDist( randomEng );
      //boxWidthDist( randomEng );
    }
    float xRel = dist01( randomEng );
    float yRel = dist01( randomEng );

    int x = static_cast<int>((maxX - width - minX)*xRel + .5f) + minX;
    int y = static_cast<int>((maxY - height - minY)*yRel + .5f) + minY;

    sassert( x >= minX );
    sassert( y >= minY );
    sassert( x + width <= maxX );
    sassert( y + height <= maxY);

    float boxTypeRandom = typeDist( randomEng );
    float dir = 0.f;
    if( boxTypeRandom < dxdyBoxWeight )
    {
      sassert( boxConf.useDxDy || boxConf.useDxDyMag );
      dir = directionDist( randomEng );
      bool useMag = false;
      if( boxConf.useDxDy && boxConf.useDxDyMag )
      {
        useMag = ( diceDist( randomEng ) > 0 );
      }
      else if( boxConf.useDxDyMag ) useMag = true;
      if( !boxConf.useAbsDxDy && !boxConf.useAbsDxDyMag &&!boxConf.useQuanAbsDxDy && !boxConf.useQuanDxDy)
        featcont.addDxdyBoxFeature( x, y,width, height, dir, useMag );
      else
        featcont.addAbsDxdyBoxFeature( x, y,width, height, dir, useMag, false );

    }
    else if ( boxTypeRandom < dxdyBoxWeight + absdxdyBoxWeight )
    {
      sassert( boxConf.useAbsDxDy || boxConf.useAbsDxDyMag );
      dir = directionDist( randomEng );
      bool useMag = false;
      if( boxConf.useAbsDxDy && boxConf.useAbsDxDyMag )
      {
        useMag = ( diceDist( randomEng ) > 0 );
      }
      else if( boxConf.useAbsDxDyMag ) useMag = true;
   
      featcont.addAbsDxdyBoxFeature( x, y,width, height, dir, useMag, true );
    }
    else if( boxTypeRandom < dxdyBoxWeight + absdxdyBoxWeight + quandxdyBoxWeight )
    {
      sassert (boxConf.useQuanAbsDxDy || boxConf.useQuanDxDy);
      float minDir = directionDist( randomEng );
      float maxDir = directionDist( randomEng );
      bool useAbs = false;
      if( boxConf.useQuanAbsDxDy && boxConf.useQuanDxDy )
      {
        useAbs = ( diceDist( randomEng ) > 0 );
      }
      else if( boxConf.useQuanAbsDxDy ) useAbs = true;

      featcont.addQuanBoxFeature( x, y,width, height, minDir, maxDir, useAbs );
    }
    else if( boxTypeRandom < dxdyBoxWeight + absdxdyBoxWeight + quandxdyBoxWeight + dxdydxydyxBoxWeight )
    {
      dir = directionDist( randomEng )*.5f;
      featcont.addBoxFeature( BoxDxDyDxyDyx( x, y, width, height, D_X, dir, true ) );
    }
    else if (boxTypeRandom < dxdyBoxWeight + absdxdyBoxWeight + quandxdyBoxWeight + dxdydxydyxBoxWeight+sumBoxNormWeight)
    {
      int channelIdx = channelDistNorm(randomEng);
      icf::ChannelType c = DIR_0;
      if (channelIdx == 1)
        c = DIR_1;
      if (channelIdx == 2)
        c = DIR_2;
      if (channelIdx == 3)
        c = DIR_3;
      if (channelIdx == 4)
        c = DIR_4;
      if (channelIdx == 5)
        c = DIR_5;

      featcont.addSumBoxNormFeature(x, y, width, height, c);
    }
    else
    {
      int channelIdx = channelDist( randomEng );
      featcont.addSumBoxFeature( x, y, width, height, usedChannelTypes[ channelIdx ]);
    }
  }
}

int vca::icf::BoxFeatureFactory::getRandomCumIdx(float v, const vector<int>& cumValues)
{
  sassert( cumValues.size() > 0 )
  sassert( v >= 0.f );
  sassert( v < 1.f );
  auto it = std::lower_bound (cumValues.begin(), cumValues.end(), v * cumValues.back());  
  sassert( cumValues.end() != it );
  return static_cast<int>( it - cumValues.begin() );
}

void vca::icf::BoxFeatureFactory::generateFeaturesGrid(FeatureContainer& featcont, int n, int maxX, int maxY, float paddingFactor,
  float intersectionFactor)
{
  auto paddingpixelheight = static_cast<int>(paddingFactor * maxY);
  auto paddingpixelwidth = static_cast<int>(paddingFactor * maxX);
  generateFeaturesGrid(featcont, n, paddingpixelwidth, paddingpixelheight, maxX - paddingpixelwidth, maxY - paddingpixelheight, intersectionFactor);
}

void vca::icf::BoxFeatureFactory::generateFeaturesGrid(FeatureContainer& featcont, int n, int minX, int minY, 
  int maxX, int maxY,   float intersectionFactor)
{

    auto height = maxY - minY;
    auto width = maxX - minX;

    auto nChannels = dxdyBoxWeight+sumBoxWeight+absdxdyBoxWeight+quandxdyBoxWeight;
    auto numperchannel = n / nChannels;
    if (numperchannel == 0)
      numperchannel = 1;
    auto heightwidthratio = static_cast<float>(height)/width;
    auto nheight = static_cast<int>(sqrt(heightwidthratio * numperchannel) + 0.5f);
    if (nheight == 0)
      nheight = 1;
    auto nwidth = numperchannel / nheight;
    if (nwidth == 0)
      nwidth = 1;
    auto haarwidth = static_cast<int>(width/(nwidth - nwidth * intersectionFactor + intersectionFactor ));
    auto haarheight = static_cast<int>(height/(nheight - nheight * intersectionFactor + intersectionFactor ));
    if (haarwidth == 0)
      haarwidth = 1;
    if (haarheight == 0)
      haarheight = 1;
    auto haaroffsetw = static_cast<int>((1.f-intersectionFactor)*haarwidth);
    auto haaroffseth = static_cast<int>((1.f-intersectionFactor)*haarheight);
    if (haaroffseth == 0)
      haaroffseth = 1;
    if (haaroffsetw == 0)
      haaroffsetw = 1;

   // cout << "num: " << num << " numperchannel: " << numperchannel << " nheight: " << nheight << " nwidht: " << nwidth;
  //  cout << " haarwidth: " << haarwidth << " haarheight: " << haarheight << " haaroffsetw:" << haaroffsetw << " haaroffseth:" << haaroffseth << endl;

    for( auto c = 0; c < nChannels; ++c )
    {
        for( auto x = 0; x < nwidth; ++x )
        {
          for( auto y = 0; y < nheight; ++y )
          {
            
            auto rx = static_cast<int>( haaroffsetw * x + minX );
            auto ry = static_cast<int>( haaroffseth * y + minY );
            auto rw = haarwidth;
            auto rh = haarheight;

            float dir = 0.f;
            if( c < dxdyBoxWeight )
            {
              sassert( boxConf.useDxDy || boxConf.useDxDyMag );
              dir = c * static_cast<float>(M_PI)/dxdyBoxWeight;
              bool useMag = false;
              if( boxConf.useDxDyMag ) useMag = true;
              if( !boxConf.useAbsDxDy && !boxConf.useAbsDxDyMag &&!boxConf.useQuanAbsDxDy && !boxConf.useQuanDxDy)
                featcont.addDxdyBoxFeature(rx, ry, rw, rh, dir, useMag);
              else
                featcont.addAbsDxdyBoxFeature(rx, ry, rw, rh, dir, useMag, false );

            }
            else if ( c < dxdyBoxWeight + absdxdyBoxWeight )
            {
              sassert( boxConf.useAbsDxDy || boxConf.useAbsDxDyMag );
              dir = (c-dxdyBoxWeight) * static_cast<float>(M_PI)/absdxdyBoxWeight;
              bool useMag = false;
              if( boxConf.useAbsDxDyMag ) useMag = true;

              featcont.addAbsDxdyBoxFeature(rx, ry, rw, rh, dir, useMag, true );
            }
            else if( c < dxdyBoxWeight + absdxdyBoxWeight + quandxdyBoxWeight )
            {
              sassert (boxConf.useQuanAbsDxDy || boxConf.useQuanDxDy);
              float minDir =(c-dxdyBoxWeight-absdxdyBoxWeight-0.5f) * static_cast<float>(M_PI)/quandxdyBoxWeight;
              float maxDir =(c-dxdyBoxWeight-absdxdyBoxWeight+0.5f) * static_cast<float>(M_PI)/quandxdyBoxWeight;
              bool useAbs = false;
              if( boxConf.useQuanAbsDxDy ) useAbs = true;

              featcont.addQuanBoxFeature(rx, ry, rw, rh, minDir, maxDir, useAbs );
            }
            else
            {
              int channelIdx = (c-dxdyBoxWeight-absdxdyBoxWeight-quandxdyBoxWeight) ;
              featcont.addSumBoxFeature( rx, ry, rw, rh, usedChannelTypes[ channelIdx ]);
            }
          }
        }
     }
}

