#include "i2c.h"

short int getSensorData(int high, int low)
{
  short int raw_sensor_data = (wiringPiI2CReadReg8(fd, high) << 8) | wiringPiI2CReadReg8(fd, low);
  return raw_sensor_data;
}