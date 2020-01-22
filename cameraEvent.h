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

#ifndef UNTITLED_CAMERAEVENT_H
#define UNTITLED_CAMERAEVENT_H

#include "main_training.h"
#include "globals.h"
#include "camerasettings.h"
#include <thread>

// Compares the a frame grabbed to the threshold and calculates the number of pixels for each line below the threshold.
__global__ void aboveThresholdCalc(const uint32_t* thresholdLine, const uint8_t* b, uint32_t* c, int N);

// Event handler used with software triggering.
// Sets the global variables for the pixels blocked on each camera.
class SoftwareTriggerImageEventHandler : public ImageEventHandler_t
{
public:
    virtual void OnImageGrabbed( Camera_t& camera, const GrabResultPtr_t& ptrGrabResult);
};

// Event handler used with continous acquisition.
// Only prints information for debugging.
class ContinousAcquisitionImageEventHandler : public ImageEventHandler_t
{
public:
    virtual void OnImageGrabbed( Camera_t& camera, const GrabResultPtr_t& ptrGrabResult);
};

#endif //UNTITLED_CAMERAEVENT_H
