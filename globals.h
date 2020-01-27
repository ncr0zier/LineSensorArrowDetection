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

#ifndef UNTITLED_GLOBALS_H
#define UNTITLED_GLOBALS_H

#include "BaselineData.h"
#include <condition_variable>
#include <mutex>

// Changes needed to more reliabled test for impacts.
// Wait for one second until continuing to collect frames after drawing a point.
// When testing for impacts it's difficult to see a single pixel from a distance.
#define IMPACT_TESTING 0

// The path where DB files are stored.
#define DB_PATH "/home/nathan/SQLiteDBs"

// Global Variables for holding the average pixel where an object was detected
// pixelCamera0 - average pixel where object was detected on L45
// pixelCamera1 - average pixel where object was detected on L90
// Set By SoftwareTriggerEventHandler::OnImageGrabbed.
// Used in the main_training and main_testing_continous main loop.
double pixelCamera0;
double pixelCamera1;

// Pointer to the current frame grabbed stored on
// GPU memory for images grabbed from both cameras.
// Set by SoftwareTriggerEventHandler::OnImageGrabbed.
// Used in the GPU function vsub to compare against aboveThresholdLine
uint8_t * grabResult_d[2];

// Counts the number of pixels above the object detection threshold in a frame.
// (_d = GPU memory) (_h = host memory)
// Used SoftwareTriggerEventHandler::OnImageGrabbed.
// Calculated by the GPU function vsub to compare against aboveThresholdLine
uint32_t * aboveThresholdCount_d[2];
uint32_t * aboveThresholdCount_h[2];

// Holds the threshold for each pixel read from an SQLite file.
// (_d = GPU memory) (_h = host memory)
BaselineData * Baseline_h[2], *Baseline_d[2];

// bool, mutex and condition_variable arrays
// The camera event handlers run in seperate threads
// the main function's thread needs to be blocked until camera event handlers finish.
bool cameraEventComplete[2];
std::mutex m[2];
std::condition_variable cv[2];

#endif //UNTITLED_GLOBALS_H
