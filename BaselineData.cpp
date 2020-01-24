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

#include "BaselineData.h"

void initBaselineData_h(struct BaselineData *& data, uint32_t gain, uint32_t exposure_time){
    data = (struct BaselineData *) malloc(sizeof(struct BaselineData));
    if(data == NULL) {
        std::cerr << "Could not allocate memory for a Baseline struct. Aborting.\n";
        std::abort();
    }
    data->gain = gain;
    data->exposure_time = exposure_time;
    memset(data->minLine, 255, LINE_BYTES_UINT32);
    memset(data->maxLine, 0, LINE_BYTES_UINT32);
}

void initBaselineData_d(struct BaselineData *& data){
    HIP_CHECK(hipMalloc(&data,sizeof(struct BaselineData)));
    HIP_CHECK(hipMemset(data->minLine,255,LINE_BYTES_UINT32));
    HIP_CHECK(hipMemset(data->maxLine,0,LINE_BYTES_UINT32));
}

void copyBaselineData_DeviceToHost(struct BaselineData *& GPU_h, struct BaselineData *& GPU_d){
    uint32_t gainTemporary = GPU_h->gain;
    uint32_t exposureTemporary = GPU_h->exposure_time;
    HIP_CHECK(hipMemcpy(GPU_h, GPU_d, sizeof(struct BaselineData), hipMemcpyDeviceToHost));
    GPU_h->gain = gainTemporary;
    GPU_h->exposure_time = exposureTemporary;
}

void copyBaselineData_HostToDevice(struct BaselineData *& GPU_h, struct BaselineData *& GPU_d){
    HIP_CHECK(hipMemcpy(GPU_h, GPU_d, sizeof(struct BaselineData), hipMemcpyHostToDevice));
}

void writeBaselineToDB(struct BaselineData *& data, const char * filename, const char * cameraName){
    // Open the SQLite3 file.
    sqlite3 *db;
    SQLite3_CHECK(sqlite3_open(filename, &db),db);

    // Create the table for baseline data if it doesn't already exist.
    sqlite3_stmt *stmt;
    SQLite3_CHECK(sqlite3_prepare_v2(db,CREATE_TABLE_STATEMENT,-1,&stmt,NULL),db);
    SQLite3_CHECK(sqlite3_step(stmt),db);
    SQLite3_CHECK(sqlite3_finalize(stmt),db);

    // Get the current datetime.
    SQLite3_CHECK(sqlite3_prepare_v2(db,"SELECT datetime('now','localtime');",-1,&stmt,NULL),db);
    SQLite3_CHECK(sqlite3_step(stmt),db);
    std::string currentDatetime = reinterpret_cast<const char*>(sqlite3_column_text   (stmt,0));
    SQLite3_CHECK(sqlite3_finalize(stmt),db);

    // Doing multiple inserts inside of a transaction is faster.
    // https://stackoverflow.com/questions/1711631/improve-insert-per-second-performance-of-sqlite
    SQLite3_CHECK(sqlite3_prepare_v2(db,"BEGIN TRANSACTION",-1,&stmt,NULL),db);
    SQLite3_CHECK(sqlite3_step(stmt),db);
    SQLite3_CHECK(sqlite3_finalize(stmt),db);

    // Insert a row for each pixel in the SQLite file using a prepared statement.
    SQLite3_CHECK(sqlite3_prepare_v2(db,INSERT_TABLE_STATEMENT,-1,&stmt,NULL),db);
    for(uint32_t i = 0; i < PIXELS_PER_LINE; i++) {
        // Bind variables to prepared statement.
        SQLite3_CHECK(sqlite3_bind_text  (stmt,1,cameraName,-1,NULL),db);
        SQLite3_CHECK(sqlite3_bind_int   (stmt,2,i),db);
        SQLite3_CHECK(sqlite3_bind_int   (stmt,3,data->gain),db);
        SQLite3_CHECK(sqlite3_bind_int   (stmt,4,data->exposure_time),db);
        SQLite3_CHECK(sqlite3_bind_int   (stmt,5,data->avgLine[i]),db);
        SQLite3_CHECK(sqlite3_bind_int   (stmt,6,data->minLine[i]),db);
        SQLite3_CHECK(sqlite3_bind_int   (stmt,7,data->maxLine[i]),db);
        SQLite3_CHECK(sqlite3_bind_double(stmt,8,data->stdDevLine[i]),db);
        SQLite3_CHECK(sqlite3_bind_int   (stmt,9,data->thresholdLine[i]),db);
        SQLite3_CHECK(sqlite3_bind_text  (stmt,10,currentDatetime.c_str(),-1,NULL),db);
        //std::cout << "Average: " << data->avgLine[i] << std::endl;
        //std::cout << sqlite3_expanded_sql(stmt) << std::endl;
        // Execute Statement.
        SQLite3_CHECK(sqlite3_step(stmt),db);

        // Reset Statement to use again.
        SQLite3_CHECK(sqlite3_reset(stmt),db);
        }
    // Cleanup the statement.
    SQLite3_CHECK(sqlite3_finalize(stmt),db);

    // End the transaction.
    SQLite3_CHECK(sqlite3_prepare_v2(db,"END TRANSACTION",-1,&stmt,NULL),db);
    SQLite3_CHECK(sqlite3_step(stmt),db);
    SQLite3_CHECK(sqlite3_finalize(stmt),db);

    // Close the database file.
    SQLite3_CHECK(sqlite3_close(db),db);
}

void readBaselineFromDB(struct BaselineData *& data, const char * filename, const char * cameraName) {
    // Open the SQLite3 file.
    sqlite3 *db;
    SQLite3_CHECK(sqlite3_open(filename, &db),db);

    // Generate an SQL statement to get the datetimes with matching gain and exposure times.
    sqlite3_stmt *stmt;
    SQLite3_CHECK(sqlite3_prepare_v2(db,DISTICT_BASELINE_STATEMENT,-1,&stmt,NULL),db);
    SQLite3_CHECK(sqlite3_bind_text  (stmt,1,cameraName,-1,NULL),db);
    SQLite3_CHECK(sqlite3_bind_int   (stmt, 2, CAMERA_GAIN), db);
    SQLite3_CHECK(sqlite3_bind_int   (stmt,3,CAMERA_EXPOSURE_TIME),db);

    // Populate a vector of datetimes for baselines returned by the above SQL query.
    std::vector<std::string> collectionTimes;
    while(SQLite3_CHECK(sqlite3_step(stmt),db) != SQLITE_DONE){
        collectionTimes.push_back(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt,0))));
    }

    // Cleanup that statement.
    SQLite3_CHECK(sqlite3_finalize(stmt),db);

    // Display the list of all times collected.
    std::cout << "Choose a baseline to load from the SQLite3 file by datetime collected.\n";
    uint32_t counter = 0;
    for (std::string time : collectionTimes){
        std::cout << counter++ << ": " << time << std::endl;
    }

    // Get the choice for the baseline.
    uint32_t choice = 0;
    do{
        std::cout << "Choose a baseline by time collected: ";
        std::cin  >> choice;
        std::cout << std::endl;
    }while(choice >= collectionTimes.size());

    // Prepare the statement for loading the selected baseline.
    SQLite3_CHECK(sqlite3_prepare_v2(db,SELECT_TABLE_STATEMENT,-1,&stmt,NULL),db);
    SQLite3_CHECK(sqlite3_bind_text(stmt,1,cameraName,-1,NULL),db);
    SQLite3_CHECK(sqlite3_bind_text(stmt,2,collectionTimes[choice].c_str(),-1,NULL),db);
    SQLite3_CHECK(sqlite3_bind_int(stmt, 3, CAMERA_GAIN), db);
    SQLite3_CHECK(sqlite3_bind_int(stmt,4,CAMERA_EXPOSURE_TIME),db);

    // Run the prepared statement loading the baseline information into a Baseline struct.
    // There isn't a rigorous way to error check column statements, but if something is wrong
    // it should show up somewhere else.
    uint32_t pixel;
    uint32_t loopCount = 0;
    while(SQLite3_CHECK(sqlite3_step(stmt),db) != SQLITE_DONE)
    {
        pixel                      = sqlite3_column_int   (stmt,0);
        data->avgLine      [pixel] = sqlite3_column_int   (stmt,1);
        data->minLine      [pixel] = sqlite3_column_int   (stmt,2);
        data->maxLine      [pixel] = sqlite3_column_int   (stmt,3);
        data->stdDevLine   [pixel] = sqlite3_column_double(stmt,4);
        data->thresholdLine[pixel] = sqlite3_column_int   (stmt,5);
        loopCount++;
    }

    // Extra check to see if this has been run PIXELS_PER_LINE times.
    if(loopCount != PIXELS_PER_LINE) {
        std::cerr << "Step executed: " << loopCount << " times. Baseline Data was not loaded for each pixel. Aborting."
                  << std::endl;
        std::abort();
    }

    // Cleanup the statement.
    SQLite3_CHECK(sqlite3_finalize(stmt),db);

    // Close the database file.
    SQLite3_CHECK(sqlite3_close(db),db);
}

void baselineGPUCalculation(struct BaselineData * GPU_h, uint8_t *frames_d) {
    // Allocate the BaselineData struct on the device.
    BaselineData * GPU_d;
    initBaselineData_d(GPU_d);

    // Calculate the sum of all pixels grabbed and use the result to calculate the average.
    uint32_t * sumLine;
    HIP_CHECK(hipMalloc(&sumLine, LINE_BYTES_UINT32));
    HIP_CHECK(hipMemset(sumLine,0,LINE_BYTES_UINT32));
    hipLaunchKernelGGL(lineSumCalc, dim3(NUM_SAMPLES * IMAGE_HEIGHT), dim3(PIXELS_PER_LINE), 0, 0, frames_d, GPU_d,
                       sumLine, NUM_DATAPOINTS);
    HIP_CHECK(hipGetLastError());
    hipLaunchKernelGGL(lineAvgCalc, dim3(1), dim3(PIXELS_PER_LINE), 0, 0, GPU_d, sumLine, PIXELS_PER_LINE);
    HIP_CHECK(hipGetLastError());
    HIP_CHECK(hipFree(sumLine));

    // Calculate the standard deviation for each pixel. The intermediate results are stored in intermediateLine.
    uint32_t * intermediateLine;
    HIP_CHECK(hipMalloc(&intermediateLine, LINE_BYTES_UINT32));
    HIP_CHECK(hipMemset(intermediateLine, 0,LINE_BYTES_UINT32));
    hipLaunchKernelGGL(lineStdDevStep1Calc, dim3(NUM_SAMPLES * IMAGE_HEIGHT), dim3(PIXELS_PER_LINE), 0, 0, frames_d,
                       GPU_d, intermediateLine, NUM_DATAPOINTS);
    HIP_CHECK(hipGetLastError());
    hipLaunchKernelGGL(lineStdDevStep2Calc, dim3(1), dim3(PIXELS_PER_LINE), 0, 0, GPU_d, intermediateLine,
                       PIXELS_PER_LINE);
    HIP_CHECK(hipGetLastError());
    HIP_CHECK(hipFree(intermediateLine));

    // Subtract 5 times the standard deviation and store it in lineThresholdCalc.
    // This is used as the threshold for detecting objects.
    hipLaunchKernelGGL(lineThresholdCalc, dim3(1), dim3(PIXELS_PER_LINE), 0, 0, GPU_d, PIXELS_PER_LINE);
    HIP_CHECK(hipGetLastError());

    // Copy the BaselineData struct to the host.
    copyBaselineData_DeviceToHost(GPU_h, GPU_d);

    // GPU_d is deallocated here after being copied to the host.
    HIP_CHECK(hipFree(GPU_d));
}

__global__ void lineSumCalc(uint8_t* frames, struct BaselineData* GPU_d, uint32_t * sumLine, unsigned int N) {
    uint32_t idx = (hipBlockIdx_x * hipBlockDim_x + hipThreadIdx_x);

    if (idx < N) {
        atomicAdd(sumLine+hipThreadIdx_x, (uint32_t) frames[idx]);
        atomicMin(GPU_d->minLine+hipThreadIdx_x, (uint32_t) frames[idx]);
        atomicMax(GPU_d->maxLine+hipThreadIdx_x, (uint32_t) frames[idx]);
    }
}

__global__ void lineAvgCalc(struct BaselineData* GPU_d, uint32_t * sumLine, unsigned int N) {
    uint32_t idx = (hipBlockIdx_x * hipBlockDim_x + hipThreadIdx_x);

    if (idx < N) {
        GPU_d->avgLine[idx] = sumLine[idx]/(NUM_SAMPLES*IMAGE_HEIGHT);
    }
}

__global__ void lineStdDevStep1Calc(uint8_t* frames, struct BaselineData* GPU_d, uint32_t * intermediateLine, unsigned int N) {
    uint32_t idx = (hipBlockIdx_x * hipBlockDim_x + hipThreadIdx_x);

    if (idx < N) {
        uint32_t temp = ((uint32_t) frames[idx] - GPU_d->avgLine[hipThreadIdx_x]) * ((uint32_t)frames[idx] - GPU_d->avgLine[hipThreadIdx_x]);
        atomicAdd(intermediateLine + hipThreadIdx_x, temp);
    }
}

__global__ void lineStdDevStep2Calc(struct BaselineData* GPU_d, uint32_t * intermediateLine, unsigned int N) {
    uint32_t idx = (hipBlockIdx_x * hipBlockDim_x + hipThreadIdx_x);

    if (idx < N) {
        GPU_d->stdDevLine[idx] = sqrt(intermediateLine[idx]/(IMAGE_HEIGHT*NUM_SAMPLES-1));
    }
}

__global__ void lineThresholdCalc(struct BaselineData* GPU_d, unsigned int N) {
    uint32_t idx = (hipBlockIdx_x * hipBlockDim_x + hipThreadIdx_x);

    if (idx < N) {
        if(GPU_d->avgLine[idx] > 5*GPU_d->stdDevLine[idx]) {
            GPU_d->thresholdLine[idx] = GPU_d->avgLine[idx] - 5 * GPU_d->stdDevLine[idx];
        }
        else {
            GPU_d->thresholdLine[idx] = 0;
        }
    }
}
