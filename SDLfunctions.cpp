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

#include "SDLfunctions.h"


void sdlWindowSetup(const char * windowTitle){
    // Initialization flag
    bool success = true;

    // Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) != 0 ){
        std::cerr << "Could not initialize SDL." << std::endl << "Error: " << SDL_GetError() << std::endl;
        std::abort();
    }

    // Create window
    gWindow = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if( gWindow == NULL ){
        std::cerr << "Could not create SDL Window." << std::endl << "Error: " << SDL_GetError() << std::endl;
        std::abort();
    }

    // Create renderer for window
    gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
    if( gRenderer == NULL ){
        std::cerr << "Could not create SDL renderer." << std::endl << "Error: " << SDL_GetError() << std::endl;
        std::abort();
    }

    // Make the SDL window white.
    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
    SDL_RenderClear( gRenderer );
    SDL_RenderPresent( gRenderer );
}

void sdlCleanup(){
    // Destroy renderer.
    SDL_DestroyRenderer( gRenderer );
    gRenderer = NULL;

    // Destroy SDL window.
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;

    // Quit all SDL subsystems
    SDL_Quit();
}