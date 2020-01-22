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

// Basler sample program Grab_UsingGrabLoopThread.cpp was used as a starting point.
// Lazy Foo's tutorials were used as a starting point for SDL code.

#include "main_training.h"
#include "ScreenPositionEstimator.h"

// Namespace for using pylon objects.
using namespace Pylon;
using std::cout, std::cin, std::endl, std::cerr;

// Number of images to be grabbed.
static const uint32_t c_countOfImagesToGrab = 100000;

int main(int argc, char* argv[]){
    chdir(DB_PATH);

    // Allocate host memory.
    hostSetup();

    // Initialize host memory with baseline pixel data from an SQLite file.
    loadBaseline();

    // Allocate and initialize device memory.
    deviceSetup();

    // Load coefficients for the fitting equation from an SQLite file.
    ScreenPositionEstimator pixelEstimator;
    pixelEstimator.loadCoefficients("training.db");

    // Setup SDL and create window.
    const char * title = "Testing Continous";
    sdlWindowSetup(title);

    // SDL Event handler
    SDL_Event e;

    int exitCode = 0;
    try{

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

        // Create a camera array for two cameras.
        Camera_t cameras[2];

        // This smart pointer will receive the grab result data.
        GrabResultPtr_t ptrGrabResult[2];

        // Setup the cameras for software triggering and set the grab strategy.
        for(int i = 0; i < 2; i++){
            camSetupSoftwareTrigger(cameras[i],devices[i], tlFactory);
            // Can both camera devices be queried whether it is ready to accept the next frame trigger?
            if (cameras[i].CanWaitForFrameTriggerReady()) {

                // Start the grabbing using the grab loop thread, by setting the grabLoopType parameter
                // to GrabLoop_ProvidedByInstantCamera. The grab results are delivered to the image event handlers.
                // The GrabStrategy_OneByOne default grab strategy is used.
                cameras[i].StartGrabbing(GrabStrategy_OneByOne, GrabLoop_ProvidedByInstantCamera);
            }
            else{
                // See the documentation of CInstantCamera::CanWaitForFrameTriggerReady() for more information.
                cout << "\nThis sample can only be used with cameras that can be queried whether they are ready to accept the next frame trigger.\n";
                throw RUNTIME_EXCEPTION("CanWaitForFrameTriggerReady() failed.");
            }
        }

        // Used to hold the return value from estimatePosition.
        std::tuple<int,int> xyTuple;

        // xyTuple is split into x and y.
        uint32_t x = 0;
        uint32_t y = 0;

        while ( cameras[0].IsGrabbing() && cameras[1].IsGrabbing()){
            // The posible range for pixelCamera0 and pixelCamera1 is 0 - PIXELS_PER_LINE
            // Set outside that range to mean an object is not detected.
            pixelCamera0 = pixelCamera1 = PIXELS_PER_LINE + 1;

            // Execute a software trigger sequentially on both cameras.
            // A new thread for both cameras is created for grabbing and processing the image.
            for(int i = 0; i < 2; i++) {
                if (cameras[i].WaitForFrameTriggerReady(500, TimeoutHandling_ThrowException)) {
                    // Execute the software trigger. Wait up to 500 ms for the camera to be ready for trigger.
                    cameraEventComplete[i] = false;
                    cameras[i].ExecuteSoftwareTrigger();
                }
            }

            // Wait until the event handlers for both cameras process the frame grabbed.
            std::unique_lock<std::mutex> lk0(m[0]);
            std::unique_lock<std::mutex> lk1(m[1]);
            cv[0].wait(lk0, []{return cameraEventComplete[0];});
            cv[1].wait(lk1, []{return cameraEventComplete[1];});

            // Check to see if an object was detected calculate the point from an equation.
            // pixelCamera0 and pixelCamera1 are set by the OnImageGrabbed event handler when a frame is grabbed.
            if(pixelCamera0 < PIXELS_PER_LINE + 1 && pixelCamera1 < PIXELS_PER_LINE + 1){
                // Get the extimated (x,y) from a polynomial equation.
                xyTuple = pixelEstimator.estimatePosition(pixelCamera0,pixelCamera1);
                x = std::get<0>(xyTuple);
                y = std::get<1>(xyTuple);

                // Draw a point in blue.
                SDL_SetRenderDrawColor(gRenderer, 0, 0, 255, 255);
                SDL_RenderDrawPoint(gRenderer,x,y);
                SDL_RenderPresent(gRenderer);

                // Report the location of the point after rendering.
                cout << "Object detected. Point Drawn at (x,y) (" << x << ',' << y<< ")" << endl;
            }
        }
    }
    catch (const GenericException &e){
        // Error handling.
        cerr << "An exception occurred." << endl
             << e.GetDescription() << endl;
        exitCode = 1;

        // Remove left over characters from input buffer.
        cin.ignore(cin.rdbuf()->in_avail());
    }

    // Comment the following two lines to disable waiting on exit.
    cerr << endl << "Press Enter to exit." << endl;
    while( cin.get() != '\n');

    // Releases all pylon resources.
    PylonTerminate();

    //Free resources and close SDL
    sdlCleanup();

    // Free memory on host and device.
    hostCleanup();
    deviceCleanup();

    // Output exit code and exit.
    cout << "Exiting with code: " << exitCode << endl;
    return exitCode;
}
