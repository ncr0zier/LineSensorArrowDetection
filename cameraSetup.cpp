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

// Based on Basler C++ samples for setting up cameras.

#include "cameraSetup.h"
#include "cameraEvent.h"

void camSetup(Camera_t & camera, Pylon::CDeviceInfo & device, Pylon::CTlFactory& tlFactory){
    // Camera must be open before setting gain, and exposure time.
    camera.Open();

    // Set the camera gain and exposure time.
    camera.GainSelector.SetValue(GainSelector_All);
    camera.GainRaw.SetValue(CAMERA_GAIN);
    camera.ExposureTimeAbs.SetValue(CAMERA_EXPOSURE_TIME);

    // Chunk Mode includes extra data with the frame grabbed.
    // Needed for CRC and timestamp to be included with the frame.
    // See the Pylon API manual for details.
    if (GenApi::IsWritable(camera.ChunkModeActive)){
        camera.ChunkModeActive.SetValue(true);
    }
    else{
        throw RUNTIME_EXCEPTION( "The camera doesn't support chunk features");
    }

    // Enable time stamp chunks.
    camera.ChunkSelector.SetValue(ChunkSelector_Timestamp);
    camera.ChunkEnable.SetValue(true);

    // Enable CRC checksum chunks.
    camera.ChunkSelector.SetValue(ChunkSelector_PayloadCRC16);
    camera.ChunkEnable.SetValue(true);
}

void camSetupSoftwareTrigger(Camera_t & camera, Pylon::CDeviceInfo & device, Pylon::CTlFactory& tlFactory){
    // Attach a camera.
    camera.Attach(tlFactory.CreateDevice(device));

    // Register the standard configuration event handler for enabling software triggering.
    // The software trigger configuration handler replaces the default configuration
    // as all currently registered configuration handlers are removed by setting the registration mode to RegistrationMode_ReplaceAll.
    camera.RegisterConfiguration( new Pylon::CSoftwareTriggerConfiguration, Pylon::RegistrationMode_ReplaceAll, Pylon::Cleanup_Delete);

    // Open the camera device and set parameters used for all configurations.
    camSetup(camera,device,tlFactory);

    // Register an event handler.
    camera.RegisterImageEventHandler( new SoftwareTriggerImageEventHandler, Pylon::RegistrationMode_Append, Pylon::Cleanup_Delete);
}

void camSetupContinous(Camera_t & camera, Pylon::CDeviceInfo & device, Pylon::CTlFactory& tlFactory){
    // Attach a camera.
    camera.Attach(tlFactory.CreateDevice(device));

    // Register the standard configuration event handler for acuqiring images continously.
    // The acquire continous configuration handler replaces the default configuration
    // as all currently registered configuration handlers are removed by setting the registration mode to RegistrationMode_ReplaceAll.
    camera.RegisterConfiguration( new Pylon::CAcquireContinuousConfiguration, Pylon::RegistrationMode_ReplaceAll, Pylon::Cleanup_Delete);

    // Open the camera device and set parameters used for all configurations.
    camSetup(camera,device,tlFactory);

    // Register an event handler.
    camera.RegisterImageEventHandler( new ContinousAcquisitionImageEventHandler, Pylon::RegistrationMode_Append, Pylon::Cleanup_Delete);
}
