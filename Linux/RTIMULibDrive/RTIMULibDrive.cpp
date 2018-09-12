////////////////////////////////////////////////////////////////////////////
//
//  This file is part of RTIMULib
//
//  Copyright (c) 2014-2015, richards-tech, LLC
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of
//  this software and associated documentation files (the "Software"), to deal in
//  the Software without restriction, including without limitation the rights to use,
//  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
//  Software, and to permit persons to whom the Software is furnished to do so,
//  subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
//  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
//  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#include <RTIMULibDefs.h>
#include "RTIMULib.h"

using namespace std;

#include <iostream>
#include <fstream>
#include <signal.h>

#include "hdf5.h"

#define DATASETNAME  "imu_fused"
#define RANK  1
static hid_t       file, dataset;         /* file and dataset handles */
static hid_t       dataspace;
hid_t imu_type_id = NULL;

typedef struct {

  uint64_t timestamp_us;
  float gyro_x;
  float gyro_y;
  float gyro_z;
  float accel_x;
  float accel_y;
  float accel_z;
  float compass_x;
  float compass_y;
  float compass_z;
  float pitch;
  float roll;
  float yaw;

} imu_t;


void open_hdf5(void);
void write_data(imu_t data[100], uint64_t extent_count);
void keyboard_interrupt_initialize(void);
void keyboard_interrupt_handler(int s);

int main_()
{
    int sampleCount = 0;
    int sampleRate = 0;
    uint64_t rateTimer;
    uint64_t displayTimer;
    uint64_t now;

    //  Using RTIMULib here allows it to use the .ini file generated by RTIMULibDemo.
    //  Or, you can create the .ini in some other directory by using:
    //      RTIMUSettings *settings = new RTIMUSettings("<directory path>", "RTIMULib");
    //  where <directory path> is the path to where the .ini file is to be loaded/saved

    RTIMUSettings *settings = new RTIMUSettings("RTIMULib");

    RTIMU *imu = RTIMU::createIMU(settings);

    if ((imu == NULL) || (imu->IMUType() == RTIMU_TYPE_NULL)) {
        printf("No IMU found\n");
        exit(1);
    }

    //  This is an opportunity to manually override any settings before the call IMUInit

    //  set up IMU

    imu->IMUInit();

    //  this is a convenient place to change fusion parameters

    imu->setSlerpPower(0.02);
    imu->setGyroEnable(true);
    imu->setAccelEnable(true);
    imu->setCompassEnable(true);

    //  set up for rate timer

    rateTimer = displayTimer = RTMath::currentUSecsSinceEpoch();

    //  now just process data

    while (1) {
        //  poll at the rate recommended by the IMU

        while (imu->IMURead()) {
            RTIMU_DATA imuData = imu->getIMUData();
            sampleCount++;

            now = RTMath::currentUSecsSinceEpoch();

            //  display 10 times per second

            if ((now - displayTimer) > 100000) {
                printf("Sample rate %d: %s\r", sampleRate, RTMath::displayDegrees("", imuData.fusionPose));
                fflush(stdout);
                displayTimer = now;
            }

            //  update rate every second

            if ((now - rateTimer) > 1000000) {
                sampleRate = sampleCount;
                sampleCount = 0;
                rateTimer = now;
            }
        }
    }
}

const std::string currentDateTime()
{

  time_t now = time(0);

  struct tm tstruct;

  char buf[80];

  tstruct = *localtime(&now);

  strftime(buf, sizeof(buf), "%Y_%m_%d_%H_%M_%S", &tstruct);

  return buf;

}

void openFile(void)
{

  ofstream myfile;

  myfile.open(currentDateTime() + ".txt");
  myfile.close();

}

void writeImu(void)
{

  keyboard_interrupt_initialize();

  open_hdf5();

  int sampleCount = 0;
  //  Using RTIMULib here allows it to use the .ini file generated by RTIMULibDemo.
  //  Or, you can create the .ini in some other directory by using:
  //      RTIMUSettings *settings = new RTIMUSettings("<directory path>", "RTIMULib");
  //  where <directory path> is the path to where the .ini file is to be loaded/saved

  RTIMUSettings *settings = new RTIMUSettings("RTIMULib");

  RTIMU *imu = RTIMU::createIMU(settings);

  if ((imu == NULL) || (imu->IMUType() == RTIMU_TYPE_NULL)) {
    printf("No IMU found\n");
    exit(1);
  }

  //  This is an opportunity to manually override any settings before the call IMUInit

  //  set up IMU

  imu->IMUInit();

  //  this is a convenient place to change fusion parameters

  imu->setSlerpPower(0.02);
  imu->setGyroEnable(true);
  imu->setAccelEnable(true);
  imu->setCompassEnable(true);

  imu_t imu_data[100] = {0};

  uint64_t extent_count = 0;

  for(;;) {

    while (imu->IMURead()) {

      RTIMU_DATA imuData = imu->getIMUData();

      imu_data[sampleCount].timestamp_us = imuData.timestamp;
      imu_data[sampleCount].accel_x = imuData.accel.x();
      imu_data[sampleCount].accel_y = imuData.accel.y();
      imu_data[sampleCount].accel_z = imuData.accel.z();
      imu_data[sampleCount].gyro_x = imuData.gyro.x();
      imu_data[sampleCount].gyro_y = imuData.gyro.y();
      imu_data[sampleCount].gyro_z = imuData.gyro.z();
      imu_data[sampleCount].compass_x = imuData.compass.x();
      imu_data[sampleCount].compass_y = imuData.compass.y();
      imu_data[sampleCount].compass_z = imuData.compass.z();
      imu_data[sampleCount].pitch = (float)(imuData.fusionPose.x() * RTMATH_RAD_TO_DEGREE);
      imu_data[sampleCount].roll = (float)(imuData.fusionPose.y() * RTMATH_RAD_TO_DEGREE);
      imu_data[sampleCount].yaw = (float)(imuData.fusionPose.z() * RTMATH_RAD_TO_DEGREE);

      sampleCount++;

      if (sampleCount >= 100) {

        write_data(imu_data, extent_count);

        sampleCount = 0;

        extent_count++;

      }

    }

  }

}

int main()
{

  writeImu();

}

void write_data(imu_t data[100], uint64_t extent_count)
{

  hsize_t size[1] = {100 * (extent_count+1)};

  herr_t status = H5Dset_extent(dataset, size);

  hid_t filespace = H5Dget_space(dataset);

  hsize_t offset[1] = {100 * extent_count};

  hsize_t dims1[1] = {100};            /* data1 dimensions */

  status = H5Sselect_hyperslab(filespace, H5S_SELECT_SET, offset, NULL, dims1, NULL);

  status = H5Dwrite(dataset, imu_type_id, dataspace, filespace, H5P_DEFAULT, data);

  H5Fflush(file, H5F_SCOPE_GLOBAL);

}

void open_hdf5(void)
{

  imu_type_id = H5Tcreate(H5T_COMPOUND, sizeof(imu_t));
  H5Tinsert(imu_type_id, "timestamp_us", HOFFSET(imu_t, timestamp_us), H5T_NATIVE_UINT64);
  H5Tinsert(imu_type_id, "gyro_x", HOFFSET(imu_t, gyro_x), H5T_NATIVE_FLOAT);
  H5Tinsert(imu_type_id, "gyro_y", HOFFSET(imu_t, gyro_y), H5T_NATIVE_FLOAT);
  H5Tinsert(imu_type_id, "gyro_z", HOFFSET(imu_t, gyro_z), H5T_NATIVE_FLOAT);
  H5Tinsert(imu_type_id, "accel_x", HOFFSET(imu_t, accel_x), H5T_NATIVE_FLOAT);
  H5Tinsert(imu_type_id, "accel_y", HOFFSET(imu_t, accel_y), H5T_NATIVE_FLOAT);
  H5Tinsert(imu_type_id, "accel_z", HOFFSET(imu_t, accel_z), H5T_NATIVE_FLOAT);
  H5Tinsert(imu_type_id, "compass_x", HOFFSET(imu_t, compass_x), H5T_NATIVE_FLOAT);
  H5Tinsert(imu_type_id, "compass_y", HOFFSET(imu_t, compass_y), H5T_NATIVE_FLOAT);
  H5Tinsert(imu_type_id, "compass_z", HOFFSET(imu_t, compass_z), H5T_NATIVE_FLOAT);
  H5Tinsert(imu_type_id, "pitch", HOFFSET(imu_t, pitch), H5T_NATIVE_FLOAT);
  H5Tinsert(imu_type_id, "roll", HOFFSET(imu_t, roll), H5T_NATIVE_FLOAT);
  H5Tinsert(imu_type_id, "yaw", HOFFSET(imu_t, yaw), H5T_NATIVE_FLOAT);

  string filenamepp = "/mnt/data/imu/LSM9DS1_1/" + currentDateTime() + "_LSM9DS1.h5";

  const char *filename = filenamepp.c_str();

  hsize_t maxdims[1] = {H5S_UNLIMITED};

  hsize_t dimsf[1] = {100};

  dataspace = H5Screate_simple(RANK, dimsf, maxdims);

  file = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

  hsize_t chunk_dims[1] ={100};

  imu_t fillvalue = {0};

  hid_t cparms = H5Pcreate(H5P_DATASET_CREATE);

  herr_t status = H5Pset_chunk(cparms, RANK, chunk_dims);

  status = H5Pset_fill_value(cparms, imu_type_id, &fillvalue);

  dataset = H5Dcreate2(file, DATASETNAME, imu_type_id, dataspace, H5P_DEFAULT, cparms, H5P_DEFAULT);

}

void keyboard_interrupt_initialize(void)
{

  struct sigaction sigIntHandler;

  sigIntHandler.sa_handler = keyboard_interrupt_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  sigaction(SIGINT, &sigIntHandler, NULL);

}

void keyboard_interrupt_handler(int s)
{

  printf("Caught signal %d\n", s);
  H5Sclose(dataspace);
  H5Dclose(dataset);
  H5Fclose(file);

  exit(1);

}
