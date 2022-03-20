#include <iostream>
#include <thread>
#include <unistd.h>
#include <unistd.h>
#include <chrono>
#include "calibrate.h"
#include "i2c.h"
#include "capture.h"

/**
 * FOR NEXT TIME
 * USE PIGPIO GET BLOCK DATA TO IMPROVE i2c SPEEDS
 * https://docs.juliahub.com/PiGPIO/8aGxa/0.2.0/api/#PiGPIO.i2c_read_i2c_block_data-Tuple{Pi,Any,Any,Any}
*/
// setup file descriptor for i2c
int fd = wiringPiI2CSetup(0x68);

int main(int argc, char** argv)
{
  // wake sensor up from sleep
  wiringPiI2CWriteReg8(fd, PWR_MGMT_1, 0);
  wiringPiI2CWriteReg8(fd, 0x1B, 0);

  if (argc == 1)
  {
    while(1)
    {
      unsigned char reg_buf[1];
      reg_buf[0] = ACCEL_ZOUT_H;
      unsigned char accel_data[6];
      auto start = std::chrono::high_resolution_clock::now();
      write(fd, reg_buf, 1);
      read(fd, accel_data, 6);
      std::cout << (float)((short int)((accel_data[0] << 8) | accel_data[1])) / ACCEL_DIVISOR << "\n";
      std::cout << (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start)).count() << "\n";
    }
  }
  else
  {
    // extract sub command from args
    std::string mode = argv[1];

    // run sub command
    if ("calibrate" == mode)
    {
      calibrate();
      return 0;
    }
    else if ("capture" == mode)
    {
      capture();
    }
    else
    {
      std::cout << "else\n";
      return 0;
    }
  }

  return 0;
}