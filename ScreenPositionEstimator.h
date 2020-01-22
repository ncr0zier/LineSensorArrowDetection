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

#ifndef UNTITLED_SCREENPOSITIONESTIMATOR_H
#define UNTITLED_SCREENPOSITIONESTIMATOR_H

#include <tuple>
#include "globals.h"

#define SELECT_COEFFICIENTS "SELECT \
x_1,x_L45,x_L90,x_L45_2,x_L45_L90,x_L90_2,x_L45_3,x_L45_2_L90,x_L45_L90_2,x_L90_3,x_L45_4,x_L45_3_L90,x_L45_2_L90_2,x_L45_L90_3,x_L90_4, \
y_1,y_L45,y_L90,y_L45_2,y_L45_L90,y_L90_2,y_L45_3,y_L45_2_L90,y_L45_L90_2,y_L90_3,y_L45_4,y_L45_3_L90,y_L45_2_L90_2,y_L45_L90_3,y_L90_4  \
FROM coefficients WHERE timeCreated=?;"

#define DISTICT_COEFFICIENTS_STATEMENT "SELECT DISTINCT timeCreated FROM coefficients ORDER BY timeCreated DESC;"

class ScreenPositionEstimator {
private:
    double x_1;
    double x_L45;
    double x_L90;
    double x_L45_2;
    double x_L45_L90;
    double x_L90_2;
    double x_L45_3;
    double x_L45_2_L90;
    double x_L45_L90_2;
    double x_L90_3;
    double x_L45_4;
    double x_L45_3_L90;
    double x_L45_2_L90_2;
    double x_L45_L90_3;
    double x_L90_4;
    double y_1;
    double y_L45;
    double y_L90;
    double y_L45_2;
    double y_L45_L90;
    double y_L90_2;
    double y_L45_3;
    double y_L45_2_L90;
    double y_L45_L90_2;
    double y_L90_3;
    double y_L45_4;
    double y_L45_3_L90;
    double y_L45_2_L90_2;
    double y_L45_L90_3;
    double y_L90_4;
public:
    // Load the coefficients from an SQLite3 file.
    void loadCoefficients(const char * filename);

    // Estimate the point using a polynomial approximation and pixel values.
    std::tuple<uint32_t,uint32_t> estimatePosition(double s0 = pixelCamera0, double s1 = pixelCamera1);
};


#endif //UNTITLED_SCREENPOSITIONESTIMATOR_H
