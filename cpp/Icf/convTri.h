#pragma once
#include <vector>
namespace vca
{
  namespace icf
  {
    namespace  convTri
    {

      //convTri1Y, convTriY and convTri: adapted from Dollars Toolbox convConst.cpp
      // convolve one column of I by a [1 p 1] filter
      inline void convTri1X(float *I, float *O, int w, float p)
      {
        int j = 0;
        O[j] = (1 + p) * I[j] + I[j + 1];
        j++;
        for (; j < w - 1; j++)
        {
          O[j] = I[j - 1] + p * I[j] + I[j + 1];
        }
        O[j] = I[j - 1] + (1 + p) * I[j];
      }

      // convolve I by a [1 p 1] filter 
      inline void convTri1(float *I, float *O, int w, int h, float r)
      {
        float p = 12.f / r / (r + 2.f) - 2.f;

        const float nrm = 1.0f / ((p + 2)*(p + 2)); 

        std::vector<float> T(w, 0);
        float *Il, *Im, *Ir;

        for (int i = 0; i<h; i++)
        {
          Ir = I + i*w;
          Im = Il = Ir;
          if (i>0)
            Il -= w;
          if (i < h - 1)
            Ir += w;
          for (int j = 0; j < w; j++)
            T[j] = nrm*(Il[j] + p*Im[j] + Ir[j]);
          convTri1X(&T[0], O, w, p);
          O += w;
        }
      }


      // convolve one column of I by a 2rx1 triangle filter
      inline void convTriX(float *I, float *O, int w, int r)
      {
        r++;
        float t = I[0];
        float u = I[0];

        int r0 = r - 1;
        int r1 = r + 1;
        int r2 = 2 * w - r;

        int w0 = r + 1;
        int w1 = w - r + 1;
        int w2 = w;

        for (int j = 1; j < r; j++)
        {
          t += I[j];
          u += t;
        }
        u = 2 * u - t;
        t = 0;
        O[0] = u;
        int j = 1;
        for (; j < w0; j++)
        {
          t += I[r - j] + I[r0 + j] - 2 * I[j - 1];
          u += t;
          O[j] = u;
        }

        for (; j < w1; j++)
        {
          t += I[j - r1] + I[r0 + j] - 2 * I[j - 1];
          u += t;
          O[j] = u;
        }
        for (; j < w2; j++)
        {
          t += I[j - r1] + I[r2 - j] - 2 * I[j - 1];
          u += t;
          O[j] = u;
        }
      }


      // convolve I by a 2rx1 triangle filter
      inline void convTri(float *I, float *O, int w, int h, int r)
      {
        r++;
        float nrm = 1.0f / (r*r*r*r);

        std::vector<float> T(w, 0);
        std::vector<float> U(w, 0);

        // initialize T and U
        for (int j = 0; j < w; j++)
        {
          T[j] = I[j];
          U[j] = I[j];
        }
        for (int i = 1; i < r; i++)
        {
          for (int j = 0; j < w; j++)
          {
            T[j] += I[j + i*w];
            U[j] += T[j];
          }
        }
        for (int j = 0; j < w; j++)
        {
          U[j] = nrm * (2 * U[j] - T[j]);
          T[j] = 0;
        }
        // prepare and convolve each row in turn
        convTriX(&U[0], O, w, r - 1);
        O += w;
        for (int i = 1; i<h; i++)
        {
          float *It = I + (i - 1 - r)*w;
          if (i <= r)
          {
            It = I + (r - i)*w;
          }
          float *Im = I + (i - 1)*w;
          float *Ib = I + (i - 1 + r)*w;
          if (i > h - r)
          {
            Ib = I + (2 * h - r - i)*w;
          }
          for (int j = 0; j < w; j++)
          {
            T[j] += It[j] + Ib[j] - 2 * Im[j];
            U[j] += nrm * T[j];
          }
          convTriX(&U[0], O, w, r - 1);
          O += w;
        }
      }

    }
  }
}

