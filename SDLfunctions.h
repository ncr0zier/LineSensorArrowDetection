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

#ifndef UNTITLED_SDLFUNCTIONS_H
#define UNTITLED_SDLFUNCTIONS_H

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 550;

// Global variable for the SDL window.
SDL_Window* gWindow = NULL;

// Global variable for the SDL renderer.
SDL_Renderer* gRenderer = NULL;

// Setup an SDL window.
void sdlWindowSetup(const char *);

// Shut down SDL.
void sdlCleanup();


#endif //UNTITLED_SDLFUNCTIONS_H
