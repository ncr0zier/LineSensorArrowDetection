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

// Basler Grab_ChunkImage.cpp sample was used as a starting point for the baseline collection program.

#include "BaselineData.h"
#include "camerasettings.h"
#include "cameraSetup.h"
#include "filenames.h"

void grabLoop(Camera_t & camera, uint8_t * frames_d);

// Namespace for using cout.
using namespace std;

// Namespace for using pylon objects.
using namespace Pylon;

// Number of images to be grabbed.
// Camera.StopGrabbing() is called automatically by the RetrieveResult() method
// when c_countOfImagesToGrab images have been retrieved.
static const uint32_t c_countOfImagesToGrab = NUM_SAMPLES;

int main(int argc, char* argv[])
{
    // Return 0 for normal execution.
    // Return 1 if an exception is caught.
    int exitCode = 0;

    // Change the CWD once to where the SQLite3 files are kept.
    chdir(DB_PATH);

    try
    {
        // Before using any pylon methods, the pylon runtime must be initialized.
        PylonInitialize();

        // Only look for cameras supported by Camera_t
        CDeviceInfo info;
        info.SetDeviceClass( Camera_t::DeviceClass());

        // Get the transport layer factory.
        CTlFactory& tlFactory = CTlFactory::GetInstance();

        // Get all attached devices and exit application if no device is found.
        // There should be two camera's attached with user defined names L45 and L90.
        DeviceInfoList_t devices;
        if( tlFactory.EnumerateDevices(devices) == 0){
            throw RUNTIME_EXCEPTION("No camera present.");
        }

        // Get the number of cameras to collect a baseline for.
        const int NUM_CAMERAS = devices.size();

        // Create a camera array for two cameras.
        Camera_t cameras[NUM_CAMERAS];

        // This smart pointer will receive the grab result data.
        GrabResultPtr_t ptrGrabResult[NUM_CAMERAS];

        // Create pointer for frames on the GPU.
        uint8_t *frames_d[NUM_CAMERAS];

        // Create pointer for baseline data on the host.
        BaselineData * BD[NUM_CAMERAS];

        // Calculate the baseline for both cameras sequentially.
        for(uint8_t i = 0; i < NUM_CAMERAS; i++) {
            // Set the camera up to acquire until the set number of frames are collected.
            camSetupContinous(cameras[i],devices[i], tlFactory);

            // Allocate memory on device for the frames.
            HIP_CHECK(hipMalloc(&frames_d[i], DATA_BYTES));

            // Run the grab loop copying the grabbed frames into GPU memory.
            cout << "Starting to grab frames for camera: " << i << ".\n";
            grabLoop(cameras[i],frames_d[i]);
            cout << "Finished grabbing frames for camera: " << i << ".\n";

            // Allocate memory for the BaselineData struct on the host.
            initBaselineData_h(BD[i], CAMERA_GAIN, CAMERA_EXPOSURE_TIME);

            // Process the data on the GPU and copy back to the host.
            baselineGPUCalculation(BD[i], frames_d[i]);

            // deallocate GPU memory for the frames.
            HIP_CHECK(hipFree(frames_d[i]));

            // Write the baseline data to the SQLite3 file.
            writeBaselineToDB(BD[i], DB_FILENAME, cameras[i].GetDeviceInfo().GetUserDefinedName());
        }
    }
    catch (const GenericException &e)
    {
        // Error handling.
        cerr << "An exception occurred." << endl
             << e.GetDescription() << endl;
        exitCode = 1;
    }

    // Comment the following three lines to disable waiting on exit.
    cout << endl << "Press Any key(s) and Enter to exit." << endl;
    std::string dummyVariable;
    cin >> dummyVariable;

    // Releases all pylon resources.
    PylonTerminate();

    return exitCode;
}

void grabLoop(Camera_t & camera, uint8_t * frames_d)
{
    // This smart pointer will receive the grab result data.
    GrabResultPtr_t ptrGrabResult;

    // Count the number of images grabbed for the memory offset.
    uint32_t counter = 0;

    // Start the grabbing of c_countOfImagesToGrab images.
    // The camera device is parameterized with a default configuration which
    // sets up free-running continuous acquisition.
    camera.StartGrabbing(c_countOfImagesToGrab);

    while(camera.IsGrabbing()){
        // Wait for an image and then retrieve it. A timeout of 5000 ms is used.
        // RetrieveResult calls the image event handler's OnImageGrabbed method.
        camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);

        // The result data is automatically filled with received chunk data.
        // (Note:  This is not the case when using the low-level API)
        const uint8_t *pImageBuffer = (uint8_t *) ptrGrabResult->GetBuffer();

        // Check to see if a buffer containing chunk data has been received.
        if (PayloadType_ChunkData != ptrGrabResult->GetPayloadType())
        {
            throw RUNTIME_EXCEPTION( "Unexpected payload type received.");
        }

        // Since we have activated the CRC Checksum feature, we can check
        // the integrity of the buffer first.
        // Note: Enabling the CRC Checksum feature is not a prerequisite for using
        // chunks. Chunks can also be handled when the CRC Checksum feature is deactivated.
        if(ptrGrabResult->HasCRC()) {
            if (ptrGrabResult->HasCRC() && ptrGrabResult->CheckCRC() == false) {
                throw RUNTIME_EXCEPTION("Image was damaged!");
            }
        }
        // Copy the frame to GPU memory.
        HIP_CHECK(hipMemcpy(frames_d+PIXELS_PER_LINE*IMAGE_HEIGHT*counter, pImageBuffer, PIXELS_PER_LINE*IMAGE_HEIGHT, hipMemcpyHostToDevice));
        counter++;
    }

    // Turn off chunk mode for the camera.
    camera.ChunkModeActive.SetValue(false);
}