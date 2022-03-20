#pragma once

float kalman(float U, float *Q, float *P, float *UHat, float *K);
float lowPassFilter(float previousFiltered, float currentRaw, float previousRaw);