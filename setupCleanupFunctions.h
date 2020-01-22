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


#ifndef UNTITLED_SETUPCLEANUPFUNCTIONS_H
#define UNTITLED_SETUPCLEANUPFUNCTIONS_H

// Allocate host memory.
void hostSetup();

// Initialize host memory.
void loadBaseline();

// Allocate and initialize GPU memory.
void deviceSetup();

// Deallocate host memory.
void hostCleanup();

// Deallocate device memory.
void deviceCleanup();


#endif //UNTITLED_SETUPCLEANUPFUNCTIONS_H
