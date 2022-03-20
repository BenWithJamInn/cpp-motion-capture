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
  enum SampleDataLabels { accelXLabel, accelYLabel, accelZLabel, gyroXLabel, gyroYLabel, gyroZLabel };
  float sampleData[6][SAMPLE_PERIOD];
  std::ofstream sampleCSV;

  // init previous raw data
  float previousAccelX = 0, previousAccelY = 0, previousAccelZ = 0, previousGyroX = 0, previousGyroY = 0, previousGyroZ = 0;
  // init previous filtered data
  float previousFilteredAccelX = 0, previousFilteredAccelY = 0, previousFilteredAccelZ = 1, previousFilteredGyroX = 0, previousFilteredGyroY = 0, previousFilteredGyroZ = 0;

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
      float accelX = (float)getSensorData(ACCEL_XOUT_H, ACCEL_XOUT_L);
      float accelY = (float)getSensorData(ACCEL_YOUT_H, ACCEL_YOUT_L);
      float accelZ = (float)getSensorData(ACCEL_ZOUT_H, ACCEL_ZOUT_L);
      float gyroX = (float)getSensorData(GYRO_XOUT_H, GYRO_XOUT_L);
      float gyroY = (float)getSensorData(GYRO_YOUT_H, GYRO_YOUT_L);
      float gyroZ = (float)getSensorData(GYRO_ZOUT_H, GYRO_ZOUT_L);

      // add to buffer
      sampleData[accelXLabel][sampleProgression] = accelX;
      sampleData[accelYLabel][sampleProgression] = accelY;
      sampleData[accelZLabel][sampleProgression] = accelZ;
      sampleData[gyroXLabel][sampleProgression] = gyroX;
      sampleData[gyroYLabel][sampleProgression] = gyroY;
      sampleData[gyroZLabel][sampleProgression] = gyroZ;

      // indicate we progressed
      sampleProgression++;

      #ifdef PROFILE_CAPTURE
      std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(lastRun - std::chrono::high_resolution_clock::now()).count() << "\n";
      #endif

      if (sampleProgression >= SAMPLE_PERIOD)
      {
        // save sample to csv
        sampleCSV.open(std::to_string(totalSamples) + ".cap.csv");
        
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

        // reset sample progression
        sampleProgression = 0;
        totalSamples++;
      }
    }
  }
}
