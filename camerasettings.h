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

#ifndef UNTITLED_CAMERASETTINGS_H
#define UNTITLED_CAMERASETTINGS_H

// Include files to use the pylon API
#include <pylon/PylonIncludes.h>

// Settings for using Basler GigE cameras.
#include <pylon/gige/BaslerGigEInstantCamera.h>
typedef Pylon::CBaslerGigEInstantCamera Camera_t;
typedef Pylon::CBaslerGigEImageEventHandler ImageEventHandler_t; // Or use Camera_t::ImageEventHandler_t
typedef Pylon::CBaslerGigEGrabResultPtr GrabResultPtr_t; // Or use Camera_t::GrabResultPtr_t
using namespace Basler_GigECameraParams;

// Include files used by samples.
//#include "/opt/pylon5/Samples/C++/include/ConfigurationEventPrinter.h"
//#include "/opt/pylon5/Samples/C++/include/ImageEventPrinter.h"

//Camera Settings
#define CAMERA_NAME_0 "L45"
#define CAMERA_NAME_1 "L90"
#define CAMERA_GAIN 800
#define CAMERA_EXPOSURE_TIME 24
#define PIXELS_PER_LINE 1024
#define IMAGE_HEIGHT 256

//Baseline Collection Settings
#define NUM_SAMPLES 4096

// Total number of pixel datapoints.
#define NUM_DATAPOINTS PIXELS_PER_LINE*IMAGE_HEIGHT*NUM_SAMPLES


#endif //UNTITLED_CAMERASETTINGS_H
