#include "filters.h"

// calculations: https://imgur.com/a/O0QkyP4
float kalman(float U, float *Q, float *P, float *UHat, float *K)
{
  // intial values
  static const float R = 0.00001;    // covariance
  static const float H = 1.00;  // scalar

  // calculations
  *K = *P * H / (H * *P * H + R);      // 6 // update Kalman Gain
  *UHat = *UHat + *K * (U - H * *UHat); // 7 // update estimate
  *P = (1 - *K * H) * *P + *Q;          // 8 // update error estimate
  return *UHat;
}

float lowPassFilter(float previousFiltered, float currentRaw, float previousRaw)
{
  #define COA 0.818
  #define COB 0.09 // idk

  // #define COA 0.739 // works kinda good
  // #define COB 0.13 // 300

  // #define COA 0.778 // works very well
  // #define COB 0.111 // 250
  
  // #define COA 0.478
  // #define COB 0.026

  return COA * previousFiltered + COB * currentRaw + COB * previousRaw;
}