#include <iostream>
#include <fstream>
#include <chrono>
#include "capture.h"
#include "i2c.h"
#include "filters.h"
// #define PROFILE_CAPTURE 1

void capture()
{
  unsigned int totalSamples = 1, sampleProgression = 0;
  auto lastRun = std::chrono::high_resolution_clock::now();
  float sampleData[6][SAMPLE_PERIOD];

  // loop constant data
  enum SampleDataLabels { accelXLabel, accelYLabel, accelZLabel, gyroXLabel, gyroYLabel, gyroZLabel };
  const int regLocations[6][2] = {
    { ACCEL_XOUT_H, ACCEL_XOUT_L },
    { ACCEL_YOUT_H, ACCEL_YOUT_L },
    { ACCEL_ZOUT_H, ACCEL_ZOUT_L },
    { GYRO_XOUT_H, GYRO_XOUT_L },
    { GYRO_YOUT_H, GYRO_YOUT_L },
    { GYRO_ZOUT_H, GYRO_ZOUT_L }
  };
  const int divsors[2] = {
    ACCEL_DIVISOR,
    GYRO_DIVISOR
  };

  // retrieve offset data
  std::ifstream offsetCSV;
  float offsets[6];
  offsetCSV.open("./offsets.csv");

  // check if offsets.csv exists
  if (offsetCSV.good())
  {
    std::string offset;
    for (int row; std::getline(offsetCSV, offset, '\n'); row++)
    {
      offsets[row] = std::stof(offset);
    }
  }

  while(1)
  {
    if (std::chrono::high_resolution_clock::now() - lastRun >= std::chrono::milliseconds(1))
    {
      // TODO: instead off repeated lines maybe loop?

      // set timer
      lastRun = std::chrono::high_resolution_clock::now();

      // gather data from mpu
      float sampleFrameData[6];

      for (int row = 0; row < 6; row++)
      {
        // fetch from sensor reg
        float tempMetric = getSensorData(regLocations[row][0], regLocations[row][1]);
        // apply offset
        tempMetric -= offsets[row];
        // apply divisor
        tempMetric /= divsors[row / 3];
        // add to buffer
        sampleData[row][sampleProgression] = tempMetric;
      }


      // indicate we progressed
      sampleProgression++;

      #ifdef PROFILE_CAPTURE
      std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(lastRun - std::chrono::high_resolution_clock::now()).count() << "\n";
      #endif

      if (sampleProgression >= SAMPLE_PERIOD)
      {
        std::string columnNames[] = { "ax", "ay", "az", "gx", "gy", "gz" };
        saveSampleToCSV(sampleData, 6, SAMPLE_PERIOD, columnNames, std::to_string(totalSamples));
        totalSamples++;
        sampleProgression = 0;
      }
    }
  }
}

void saveSampleToCSV(float sampleData[][SAMPLE_PERIOD], int shapeWidth, int shapeLength, std::string columnNames[], std::string name)
{
  // save sample to csv
  std::ofstream sampleCSV;
  sampleCSV.open(name + ".cap.csv");

  for (int i = 0; i < shapeWidth; i++)
    sampleCSV << columnNames[i] << ",";

  sampleCSV << "\n";
  
  for (int row = 0; row < shapeLength; row++)
  {
    for (int column = 0; column < shapeWidth; column++)
    {
      sampleCSV << sampleData[column][row];
      if (column != 5)
        sampleCSV << ",";
    }
    sampleCSV << "\n";
  }
  
  sampleCSV.close();
}
