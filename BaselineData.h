// Line Sensor Arrow Detection uses line sensors to measure the location an
// arrow hits a projector screen.
//
// Copyright (C) 2020  Nathan W. Crozier
//
// This file is part of Line Sensor Arrow Detection
//
// Line Sensor Arrow Detection is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Line Camera Arrow Detection is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Line Camera Arrow Detection.  If not, see <https://www.gnu.org/licenses/>.

#ifndef UNTITLED_BASELINEDATA_H
#define UNTITLED_BASELINEDATA_H

#include <sqlite3.h>
#include <vector>
#include <string>
#include "hip/hip_runtime.h"
#include "camerasettings.h"
#include "errorCheckingMacros.h"

// Array sizes in bytes:
#define DATA_BYTES        NUM_DATAPOINTS    * sizeof(uint8_t)
#define LINE_BYTES_UINT32 PIXELS_PER_LINE   * sizeof(uint32_t)
#define LINE_BYTES_DOUBLE PIXELS_PER_LINE   * sizeof(double)

// SQL Statements:
#define CREATE_TABLE_STATEMENT     "CREATE TABLE IF NOT EXISTS 'Baseline Data' ('cameraName' TEXT,'pixel' INTEGER, 'gain' INTEGER, 'exposure' INTEGER, 'avg' INTEGER, 'min' INTEGER, 'max' INTEGER, 'stdDev' REAL, 'fiveSigma' INTEGER, 'timeCreated' TEXT);"

#define INSERT_TABLE_STATEMENT     "INSERT INTO 'Baseline Data' VALUES(?,?,?,?,?,?,?,?,?,?);"

#define SELECT_TABLE_STATEMENT     "SELECT pixel, avg, min, max, stdDev, fiveSigma FROM 'Baseline Data' WHERE cameraName=? AND timeCreated=? AND gain=? AND exposure=?;"

#define DISTICT_BASELINE_STATEMENT "SELECT DISTINCT timeCreated FROM 'Baseline Data' WHERE cameraName=? AND gain=? AND exposure=? ORDER BY timeCreated DESC;"

// BaselineData definition for GPU and host:
struct BaselineData
{
    uint32_t  gain;
    uint32_t  exposure_time;
    uint32_t  minLine[PIXELS_PER_LINE];
    uint32_t  maxLine[PIXELS_PER_LINE];
    uint32_t  avgLine[PIXELS_PER_LINE];
    float     stdDevLine[PIXELS_PER_LINE];
    uint32_t  thresholdLine[PIXELS_PER_LINE];
};

// Host functions
void initBaselineData_h(struct BaselineData *& data, uint32_t gain, uint32_t exposure_time);
void initBaselineData_d(struct BaselineData *& data);
void copyBaselineData_DeviceToHost(struct BaselineData *& GPU_h, BaselineData *& GPU_d);
void baselineGPUCalculation(struct BaselineData * GPU_h, uint8_t *frames_d);
void writeBaselineToDB(struct BaselineData *& data, const char * filename, const char * cameraName);
void readBaselineFromDB(struct BaselineData *& data, const char * filename, const char * cameraName);
void copyBaselineData_HostToDevice(struct BaselineData *& GPU_h, struct BaselineData *& GPU_d);

// GPU functions called exclusively by baselineGPUCalculation
__global__ void lineSumCalc(uint8_t* frames, struct BaselineData* GPU_d, uint32_t * sumLine, unsigned int N);
__global__ void lineAvgCalc(struct BaselineData* GPU_d, uint32_t * sumLine, unsigned int N);
__global__ void lineStdDevStep1Calc(uint8_t* frames, struct BaselineData* GPU_d, uint32_t * intermediateLine, unsigned int N);
__global__ void lineStdDevStep2Calc(struct BaselineData* GPU_d, uint32_t * intermediateLine, unsigned int N);
__global__ void lineThresholdCalc(struct BaselineData* GPU_d, unsigned int N);

#endif //UNTITLED_BASELINEDATA_H
