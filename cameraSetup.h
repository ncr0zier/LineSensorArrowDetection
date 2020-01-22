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

#ifndef UNTITLED_CAMERASETUP_H
#define UNTITLED_CAMERASETUP_H

#include "camerasettings.h"
#include "cameraEvent.h"

// Called by the camSetupSoftwareTrigger and camSetupContinous.
// Should not be called directly.
void camSetup(Camera_t & camera, Pylon::CDeviceInfo & device, Pylon::CTlFactory& tlFactory);

// Used for training and testing a calibration.
void camSetupSoftwareTrigger(Camera_t & camera, Pylon::CDeviceInfo & device, Pylon::CTlFactory& tlFactory);

// Used for collecting the baseline.
void camSetupContinous(Camera_t & camera, Pylon::CDeviceInfo & device, Pylon::CTlFactory& tlFactory);
#endif //UNTITLED_CAMERASETUP_H
