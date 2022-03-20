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
    for (int i; std::getline(offsetCSV, offset, '\n'); i++)
    {
      offsets[i] = std::stof(offset);
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

      for (int i = 0; i < 6; i++)
      {
        // fetch from sensor reg
        float tempMetric = getSensorData(regLocations[i][0], regLocations[i][1]);
        // apply offset
        tempMetric -= offsets[i];
        // apply divisor
        tempMetric /= divsors[i / 3];
        // add to buffer
        sampleData[i][sampleProgression] = tempMetric;
      }


      // indicate we progressed
      sampleProgression++;

      #ifdef PROFILE_CAPTURE
      std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(lastRun - std::chrono::high_resolution_clock::now()).count() << "\n";
      #endif

      if (sampleProgression >= SAMPLE_PERIOD)
      {
        saveSampleToCSV(sampleData, std::to_string(totalSamples));
        totalSamples++;
        sampleProgression = 0;
      }
    }
  }
}

// TODO: add ability for shape and dynamic column names
void saveSampleToCSV(float sampleData[][SAMPLE_PERIOD], std::string name)
{
  // save sample to csv
  std::ofstream sampleCSV;
  sampleCSV.open(name + ".cap.csv");
  
  sampleCSV << "ax,ay,az,gx,gy,gz\n";
  for (int i = 0; i < SAMPLE_PERIOD; i++)
  {
    for (int label = 0; label < 6; label++)
    {
      sampleCSV << sampleData[label][i];
      if (label != 5)
        sampleCSV << ",";
    }
    sampleCSV << "\n";
  }
  
  sampleCSV.close();
}
