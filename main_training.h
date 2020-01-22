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

#ifndef UNTITLED_MAIN_TRAINING_H
#define UNTITLED_MAIN_TRAINING_H

// vector and tuple STL containers.
#include <vector>
#include <tuple>

// Include files for the HIP runtime
#include "hip/hip_runtime.h"

// Used for checking HIP errors.
#include "errorCheckingMacros.h"

// SDL2 functions.
#include "SDLfunctions.h"

// SQLite commands
#include "SQLitefunctions.h"

// Include file for the BaselineData struct
#include "BaselineData.h"

// Header files and settings for the cameras.
#include "camerasettings.h"

// Header file for setting up cameras.
#include "cameraSetup.h"

// Header file for camera event functions.
#include "cameraEvent.h"

// Global variables
#include "globals.h"

// Filenames used by the programs.
#include "filenames.h"

// Used for changing the current directory.
#include <unistd.h>

// Initializing and freeing memory functions.
#include "setupCleanupFunctions.h"

// Used for storing calibration datapoints.
struct DataPoint
{
    uint32_t x;
    uint32_t y;
    float L45;
    float L90;
};

#endif //UNTITLED_MAIN_TRAINING_H
