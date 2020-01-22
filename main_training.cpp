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

// Namespace for using pylon objects.
using namespace Pylon;
using std::cout, std::cin, std::endl, std::cerr;

int main(int argc, char* argv[]){
    chdir(DB_PATH);

    // Allocate host memory.
    hostSetup();

    // Initialize host memory with baseline pixel data from an SQLite file.
    loadBaseline();

    // Allocate and initialize device memory.
    deviceSetup();

    // Setup SDL and create window.
    const char * title = "Training";
    sdlWindowSetup(title);

    // SDL Event handler
    SDL_Event e;

    // Generate the calibration points.
    std::vector<std::tuple<int,int>> calibrationPoints;
    for(uint32_t i = 25; i < SCREEN_WIDTH; i += 25 ){
        for(uint32_t j = 25; j < SCREEN_HEIGHT; j += 25){
            calibrationPoints.push_back(std::make_tuple(i,j));
        }
    }

    // Data points are stored in a vector.
    std::vector<struct DataPoint> dataPoints;

    // This variable holds the current datapoint and is added to the dataPoints vector.
    struct DataPoint currentPoint;

    int exitCode = 0;
    try{
        // Before using any pylon methods, the pylon runtime must be initialized.
        PylonInitialize();

        //Main loop flag
        bool quit = false;

        // Only look for GigE cameras
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
                cout << "\nThis sample can only be used with cameras that can be queried whether"
                     << " they are ready to accept the next frame trigger.\n";
                throw RUNTIME_EXCEPTION("CanWaitForFrameTriggerReady() failed.");
            }
        }

        // Wait for user input to trigger the camera or exit the program.
        // The grabbing is stopped, the device is closed and destroyed automatically
        // when the camera object goes out of scope.
        char key;

        // Gain control of the mutex while in the main function scope.
        // May not be necessary, but is intended to prevent it from being modified while the program
        // is in the main function scope.
        std::scoped_lock main_lk0(m[0]);
        std::scoped_lock main_lk1(m[1]);

        // Used to index calibrationPoints.
        uint32_t n = 0;
        do {
            // Set the coordinations for the point from the calibration point.
            currentPoint.x = std::get<0>(calibrationPoints[n]);
            currentPoint.y = std::get<1>(calibrationPoints[n]);

            // Set the SDL window to be white.
            SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
            SDL_RenderClear( gRenderer );

            // Draw a point in blue.
            SDL_SetRenderDrawColor(gRenderer, 0, 0, 255, 255);
            SDL_RenderDrawPoint(gRenderer,currentPoint.x,currentPoint.y);
            SDL_RenderPresent( gRenderer );

            // Report the location of the point after rendering.
            cout << "Point Drawn at (x,y) (" << currentPoint.x << ',' << currentPoint.y << ")" << endl;

            // There should be an object in front of the screen at (x,y) blocking
            // light to the cameras when 't/T' is entered for the key.
            cout << "Enter \"t\" to trigger the cameras, \"e\" to exit,"
                 << "or any other key to draw a new point and press enter." << endl
                 << "key input: ";

            // Read a key.
            cin >> key;

            // Execute the software trigger on both cameras if the key is t.
            if ( (key == 't' || key == 'T')){
                // The posible range for pixelCamera0 and pixelCamera1 is 0 - 1024.
                // This is the pixels on the camera blocked by an object.
                pixelCamera0 = 1025;
                pixelCamera1 = 1025;

                // Execute a software trigger sequentially on both cameras.
                // A new thread for both cameras is created for grabbing and processing the image.
                for(int i = 0; i < 2; i++) {
                    if (cameras[i].WaitForFrameTriggerReady(500, TimeoutHandling_ThrowException)) {
                        // Execute the software trigger. Wait up to 500 ms for the camera to be ready for trigger.
                        cameraEventComplete[i] = 0;
                        cameras[i].ExecuteSoftwareTrigger();
                    }
                }

                // Event handlers run in a seperate thread.
                // Wait until the event handlers for both cameras process the frame.
                std::unique_lock<std::mutex> lk0(m[0]);
                std::unique_lock<std::mutex> lk1(m[1]);
                cv[0].wait(lk0, []{return cameraEventComplete[0];});
                cv[1].wait(lk1, []{return cameraEventComplete[1];});

                // Check to see if an object was detected and store the point in the vector.
                // pixelCamera0 and pixelCamera1 are set by the OnImageGrabbed event handler when a frame is grabbed.
                cout << "pixelCamera0: " << pixelCamera0 << " pixelCamera1: " << pixelCamera1 << endl;
                if(pixelCamera0 < 1025 && pixelCamera1 < 1025) {
                    currentPoint.L45 = pixelCamera0;
                    currentPoint.L90 = pixelCamera1;
                    cout << "Adding point to dataPoints vector.\n";
                    dataPoints.push_back(currentPoint);
                    cout << "Number of points collected so far: " << dataPoints.size() << endl;
                }
                else {
                    cout << "An object was not detected. Trying another (x,y) point.\n";
                }
            }

            // Increment the calibration point index,
            n++;
        }while ( (key != 'e') && (key != 'E') && n < calibrationPoints.size());

        // Write the points to an SQLite file.
        cout << "Number of points collected: " << dataPoints.size() << endl;
        if(dataPoints.size() > 0){
            cout << "Writing the dataPoints vector to an SQLite file.";
            writeDataPointsToDB(dataPoints, DB_FILENAME);
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
    cout << endl << "Press Any key(s) and Enter to exit." << endl;
    std::string dummyVariable;
    cin >> dummyVariable;

    // Free all resources used.
    PylonTerminate();
    sdlCleanup();
    hostCleanup();
    deviceCleanup();

    // Output exit code and exit.
    cout << "Exiting with code: " << exitCode << endl;
    return exitCode;
}