#pragma once
#include <vector>

void calibrate();
std::vector<float> generateOffsetCollection();
void generateOffset(float *value, int high, int low, float intendedValue);
