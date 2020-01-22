#// Line Sensor Arrow Detection uses line sensors to measure the location an
#// arrow hits a projector screen.
#//
#// Copyright (C) 2020  Nathan W. Crozier
#//
#// This file is part of Line Sensor Arrow Detection
#//
#// Line Sensor Arrow Detection is free software: you can redistribute it and/or modify
#// it under the terms of the GNU General Public License as published by
#// the Free Software Foundation, either version 3 of the License, or
#// (at your option) any later version.
#//
#// Line Camera Arrow Detection is distributed in the hope that it will be useful,
#// but WITHOUT ANY WARRANTY; without even the implied warranty of
#// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#// GNU General Public License for more details.
#//
#// You should have received a copy of the GNU General Public License
#// along with Line Camera Arrow Detection.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)
PKG_SEARCH_MODULE(SQLITE3 REQUIRED sqlite3)

find_package(Pylon QUIET)
if (NOT ${Pylon_FOUND})
    Message("Running FindPylon.cmake")
    include("${CMAKE_CURRENT_SOURCE_DIR}/FindPylon.cmake")
endif()

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIRS} ${Pylon_INCLUDE_DIRS})

add_executable(training globals.h main_training.cpp main_training.h filenames.h camerasettings.h cameraEvent.cpp cameraEvent.h SDLfunctions.cpp SDLfunctions.h SQLitefunctions.cpp SQLitefunctions.h BaselineData.cpp BaselineData.h cameraSetup.cpp cameraSetup.h errorCheckingMacros.h setupCleanupFunctions.cpp setupCleanupFunctions.h)
add_executable(testing_continous globals.h main_testing_continous.cpp main_training.h filenames.h camerasettings.h cameraEvent.cpp cameraEvent.h SDLfunctions.cpp SDLfunctions.h SQLitefunctions.cpp SQLitefunctions.h BaselineData.cpp BaselineData.h cameraSetup.cpp cameraSetup.h errorCheckingMacros.h ScreenPositionEstimator.cpp ScreenPositionEstimator.h setupCleanupFunctions.cpp setupCleanupFunctions.h)
#add_executable(baseline_test main_baselinetest.cpp BaselineData.cpp BaselineData.h errorCheckingMacros.h)
add_executable(baseline main_baseline.cpp BaselineData.cpp BaselineData.h cameraSetup.cpp cameraSetup.h cameraEvent.cpp cameraEvent.h filenames.h errorCheckingMacros.h)

TARGET_LINK_LIBRARIES(training ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SQLITE3_LIBRARIES} ${Pylon_LIBRARIES})
TARGET_LINK_LIBRARIES(testing_continous ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SQLITE3_LIBRARIES} ${Pylon_LIBRARIES})
#TARGET_LINK_LIBRARIES(baseline_test ${SQLITE3_LIBRARIES} )
TARGET_LINK_LIBRARIES(baseline ${SQLITE3_LIBRARIES} ${Pylon_LIBRARIES})
