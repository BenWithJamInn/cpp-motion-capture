#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <fstream>
#include "i2c.h"
#include "calibrate.h"

void calibrate()
{
  std::vector<float> offsets = generateOffsetCollection();
  std::cout
    << "ax: " << offsets[0] << "\n"
    << "ay: " << offsets[1] << "\n"
    << "az: " << offsets[2] << "\n"
    << "gx: " << offsets[3] << "\n"
    << "gy: " << offsets[4] << "\n"
    << "gz: " << offsets[5] << "\n";

  std::ofstream csv;
  csv.open("offsets.csv");

  for (int i = 0; i < offsets.size(); i++)
  {
    csv << offsets[i] << "\n";
  }

  csv.close();
}

// return array in order:
// ax ay az gx gy gz
std::vector<float> generateOffsetCollection()
{
  std::cout << "You are about to start calibration, please make sure the sensor is flat and still during calibration.\n";
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  std::cout << "3\n";
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  std::cout << "2\n";
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  std::cout << "1\n";
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  std::cout << "Calibrating...\n";

  float accel_xout_offset, accel_yout_offset, accel_zout_offset;
  float gyro_xout_offset, gyro_yout_offset, gyro_zout_offset;

  std::thread t2(generateOffset, &accel_xout_offset, ACCEL_XOUT_H, ACCEL_XOUT_L, ACCEL_DIVISOR, 0);
  std::thread t3(generateOffset, &accel_yout_offset, ACCEL_YOUT_H, ACCEL_YOUT_L, ACCEL_DIVISOR, 0);
  std::thread t4(generateOffset, &accel_zout_offset, ACCEL_ZOUT_H, ACCEL_ZOUT_L, ACCEL_DIVISOR, 1); // gravity
  std::thread t5(generateOffset, &gyro_xout_offset, GYRO_XOUT_H, GYRO_XOUT_L, GYRO_DIVISOR, 0);
  std::thread t6(generateOffset, &gyro_yout_offset, GYRO_YOUT_H, GYRO_YOUT_L, GYRO_DIVISOR, 0);
  std::thread t7(generateOffset, &gyro_zout_offset, GYRO_ZOUT_H, GYRO_ZOUT_L, GYRO_DIVISOR, 0);

  t2.join();
  t3.join();
  t4.join();
  t5.join();
  t6.join();
  t7.join();

  std::vector<float> result = {accel_xout_offset, accel_yout_offset, accel_zout_offset, gyro_xout_offset, gyro_yout_offset, gyro_xout_offset};

  return result;
}

void generateOffset(float *value, int high, int low, int divisor, float intendedValue)
{
  double total;

  for (int i = 0; i < CALI_SAMPLE_SIZE; i++)
  {
    float num = (float)getSensorData(high, low) / divisor;
    total += num;
  }

  *value = (total / CALI_SAMPLE_SIZE) - intendedValue;
}
