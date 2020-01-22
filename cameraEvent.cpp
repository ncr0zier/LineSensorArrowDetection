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

// Event handlers in Basler C++ samples were used as a starting point.

#include "cameraEvent.h"

using std::cout, std::endl, std::cerr;
using namespace Pylon;

__global__ void aboveThresholdCalc(const uint32_t* thresholdLine, const uint8_t* b, uint32_t* c, int N){
    uint32_t idx = (hipBlockIdx_x * hipBlockDim_x + hipThreadIdx_x);

    if (idx < N) {
        if(thresholdLine[hipThreadIdx_x] > (((uint32_t) b[idx]))) atomicAdd(c+hipThreadIdx_x,1);
    }
}

void SoftwareTriggerImageEventHandler::OnImageGrabbed(Camera_t& camera, const GrabResultPtr_t& ptrGrabResult){
    // Get the user defined name for the camera.
    // Determine which camera is being used from the user defined name and set cameraNo.
    String_t cameraName = camera.GetDeviceInfo().GetUserDefinedName();
    uint32_t cameraNo;
    if(cameraName == CAMERA_NAME_0){
        cameraNo = 0;
    }
    else if(cameraName == CAMERA_NAME_1){
        cameraNo = 1;
    }
    else{
        throw RUNTIME_EXCEPTION("No Matching Camera Name.");
    }

    // Gain control of the mutex used to block the main loop while in the camera event handler scope.
    // May not be necessary, but prevents it from being modified in another scope causing a spurious wakeup
    // before the camera event finishes executing.
    // The lock is automatically removed when the function ends.
    std::scoped_lock event_lk(m[cameraNo]);

    // The images being grabbed should have a CRC and the CRC should pass.
    if(ptrGrabResult->HasCRC()) {
        if (ptrGrabResult->CheckCRC() == false) {
            throw RUNTIME_EXCEPTION("Image failed CRC check.");
        }
    }
    else{
        throw RUNTIME_EXCEPTION("Image doesn't have CRC.");
    }

    // Reset aboveThresholdCount to zero on the GPU.
    HIP_CHECK(hipMemset(aboveThresholdCount_d[cameraNo], 0, PIXELS_PER_LINE*sizeof(uint32_t)));

    // Copy the grabbed frame to the GPU.
    HIP_CHECK(hipMemcpy(grabResult_d[cameraNo], ptrGrabResult->GetBuffer(), PIXELS_PER_LINE*IMAGE_HEIGHT, hipMemcpyHostToDevice));

    // Count the number of pixels above the threshold in the grab result and copy the result back to the host.
    hipLaunchKernelGGL(aboveThresholdCalc, dim3(IMAGE_HEIGHT), dim3(PIXELS_PER_LINE), 0, 0,
                       Baseline_d[cameraNo]->thresholdLine, grabResult_d[cameraNo], aboveThresholdCount_d[cameraNo],
                       IMAGE_HEIGHT * PIXELS_PER_LINE);
    HIP_CHECK(hipGetLastError());
    HIP_CHECK(hipMemcpy(aboveThresholdCount_h[cameraNo], aboveThresholdCount_d[cameraNo], PIXELS_PER_LINE*sizeof(uint32_t), hipMemcpyDeviceToHost));

    // Write the number of pixels detected to the terminal.
    cout << camera.GetDeviceInfo().GetUserDefinedName() <<": Object Detected at the following pixels.\n" << endl;
    uint32_t totalPixels = 0;
    uint32_t pixelSum = 0;

    // If half of the pixels in an image above the threshold, Consider an object to be blocking light to that pixel.
    // Every pixel above the threshold is given an equal weight for calculating the average pixel.
    for(int i = 0; i < PIXELS_PER_LINE; i++){
        if(aboveThresholdCount_h[cameraNo][i] > IMAGE_HEIGHT/2){
            cout << camera.GetDeviceInfo().GetUserDefinedName() << " : " << i << " : " << aboveThresholdCount_h[cameraNo][i] << endl;
            totalPixels++;
            pixelSum += i;
        }
    }

    // Set the global variables that will be written to an SQLite file for calibration.
    if(cameraName == CAMERA_NAME_0 && totalPixels > 0){
        cout << "Set pixelCamera0.\n";
        pixelCamera0 = pixelSum / totalPixels;
    }
    else if(cameraName == CAMERA_NAME_1 && totalPixels > 0){
        cout << "Set pixelCamera1.\n";
        pixelCamera1 = pixelSum / totalPixels;
    }

    // Unlock the mutex for the camera number in the main loop.
    // Only the thread for the main loop should be waiting.
    cameraEventComplete[cameraNo] = true;
    cv[cameraNo].notify_one();

    // Write the thread if and camera name for the camera event handler.
    std::cout << "Event Handler Exiting. Thread ID: " << std::this_thread::get_id() << " Camera: " << cameraName << endl;
}

// Example of a device-specific handler for image events.
void ContinousAcquisitionImageEventHandler::OnImageGrabbed( Camera_t& camera, const GrabResultPtr_t& ptrGrabResult){
    // The chunk data is attached to the grab result and can be accessed anywhere.

    // Generic parameter access:
    // This shows the access via the chunk data node map. This method is available for all grab result types.
    GenApi::CIntegerPtr chunkTimestamp( ptrGrabResult->GetChunkDataNodeMap().GetNode( "ChunkTimestamp"));

    // Access the chunk data attached to the result.
    // Before accessing the chunk data, you should check to see
    // if the chunk is readable. When it is readable, the buffer
    // contains the requested chunk data.
    if ( IsReadable( chunkTimestamp))
        cout << "OnImageGrabbed: TimeStamp (Result) accessed via node map: " << chunkTimestamp->GetValue() << endl;

    // Native parameter access:
    // When using the device-specific grab results the chunk data can be accessed
    // via the members of the grab result data.
    if ( IsReadable(ptrGrabResult->ChunkTimestamp))
        cout << "OnImageGrabbed: TimeStamp (Result) accessed via result member: " << ptrGrabResult->ChunkTimestamp.GetValue() << endl;
}
