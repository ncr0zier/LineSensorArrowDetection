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

#include "main_training.h"

using std::endl, std::cerr, std::cout;
void hostSetup(){
    // Initialize the baseline struct for both cameras on the host.
    initBaselineData_h(Baseline_h[0], CAMERA_GAIN, CAMERA_EXPOSURE_TIME);
    initBaselineData_h(Baseline_h[1], CAMERA_GAIN, CAMERA_EXPOSURE_TIME);

    // This is the array for counting the number of times each pixel was above the threshold in the grab result.
    aboveThresholdCount_h[0] = (uint32_t*)malloc(PIXELS_PER_LINE*sizeof(uint32_t));
    if(aboveThresholdCount_h[0] == NULL) {
        cerr << "Could not allocate memory for aboveThresholdCount[0]." << endl;
        abort();
    }
    aboveThresholdCount_h[1] = (uint32_t*)malloc(PIXELS_PER_LINE*sizeof(uint32_t));
    if(aboveThresholdCount_h[1] == NULL) {
        cerr << "Could not allocate memory for aboveThresholdCount[1]." << endl;
        std::abort();
    }
}

void hostCleanup(){
    // Deallocate the baseline struct and aboveThresholdCount for both cameras on the host.
    free(Baseline_h[0]);
    free(Baseline_h[1]);
    free(aboveThresholdCount_h[0]);
    free(aboveThresholdCount_h[1]);
}

void loadBaseline(){
    // Load the baseline into memory from the SQLite file.
    readBaselineFromDB(Baseline_h[0], DB_FILENAME, "L45");
    readBaselineFromDB(Baseline_h[1], DB_FILENAME, "L90");
}

void deviceSetup(){
    // Initialize the baseline struct for both cameras on the GPU.
    initBaselineData_d(Baseline_d[0]);
    initBaselineData_d(Baseline_d[1]);

    // This is an array for counting the number of times each pixel was above the threshold in the grab result.
    HIP_CHECK(hipMalloc(&aboveThresholdCount_d[0], PIXELS_PER_LINE*sizeof(uint32_t)));
    HIP_CHECK(hipMalloc(&aboveThresholdCount_d[1], PIXELS_PER_LINE*sizeof(uint32_t)));

    // Copy the struct loaded from the SQLite file in host memory.
    copyBaselineData_HostToDevice(Baseline_d[0],Baseline_h[0]);
    copyBaselineData_HostToDevice(Baseline_d[1],Baseline_h[1]);

    // Initialize memory for the grab result.
    HIP_CHECK(hipMalloc(&grabResult_d[0], PIXELS_PER_LINE*IMAGE_HEIGHT*sizeof(uint8_t)));
    HIP_CHECK(hipMalloc(&grabResult_d[1], PIXELS_PER_LINE*IMAGE_HEIGHT*sizeof(uint8_t)));
}

void deviceCleanup(){
    // Deallocate all memory used on the GPU.
    HIP_CHECK(hipFree(Baseline_d[0]));
    HIP_CHECK(hipFree(Baseline_d[1]));
    HIP_CHECK(hipFree(aboveThresholdCount_d[0]));
    HIP_CHECK(hipFree(aboveThresholdCount_d[1]));
    HIP_CHECK(hipFree(grabResult_d[0]));
    HIP_CHECK(hipFree(grabResult_d[1]));
}