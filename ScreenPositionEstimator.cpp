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

#include "ScreenPositionEstimator.h"

std::tuple<uint32_t,uint32_t> ScreenPositionEstimator::estimatePosition(double s0, double s1) {
    // Zero order terms
    // First order terms
    // Second order terms
    // Third order terms
    // Fourth order terms
    uint32_t y = y_1                 + \
                 y_L45*s0            + y_L90*s1                + \
                 y_L45_2*s0*s0       + y_L45_L90*s0*s1         + y_L90_2*s1*s1             + \
                 y_L45_3*s0*s0*s0    + y_L45_2_L90*s0*s0*s1    + y_L45_L90_2*s0*s1*s1      + y_L90_3*s1*s1*s1        + \
                 y_L45_4*s0*s0*s0*s0 + y_L45_3_L90*s0*s0*s0*s1 + y_L45_2_L90_2*s0*s0*s1*s1 + y_L45_L90_3*s0*s1*s1*s1 + y_L90_4*s1*s1*s1*s1;

    uint32_t x = x_1                 + \
                 x_L45*s0            + x_L90*s1                + \
                 x_L45_2*s0*s0       + x_L45_L90*s0*s1         + x_L90_2*s1*s1             + \
                 x_L45_3*s0*s0*s0    + x_L45_2_L90*s0*s0*s1    + x_L45_L90_2*s0*s1*s1      + x_L90_3*s1*s1*s1        + \
                 x_L45_4*s0*s0*s0*s0 + x_L45_3_L90*s0*s0*s0*s1 + x_L45_2_L90_2*s0*s0*s1*s1 + x_L45_L90_3*s0*s1*s1*s1 + x_L90_4*s1*s1*s1*s1;

    return std::make_tuple(x,y);
}

void ScreenPositionEstimator::loadCoefficients(const char *filename) {
    // Open the database file.
    sqlite3 *db;
    SQLite3_CHECK(sqlite3_open(filename, &db),db);

    // Generate an SQL statement to get all the times coefficients have been written to a database file.
    sqlite3_stmt *stmt;
    SQLite3_CHECK(sqlite3_prepare_v2(db,DISTICT_COEFFICIENTS_STATEMENT,-1,&stmt,NULL),db);

    // Get the most recent time.
    SQLite3_CHECK(sqlite3_step(stmt),db);
    std::string mostRecentTime = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt,0)));

    // Cleanup that statement.
    SQLite3_CHECK(sqlite3_finalize(stmt),db);

    // Prepare the statement for loading coefficients.
    SQLite3_CHECK(sqlite3_prepare_v2(db,SELECT_COEFFICIENTS,-1,&stmt,NULL),db);
    SQLite3_CHECK(sqlite3_bind_text(stmt,1,mostRecentTime.c_str(),-1,NULL),db);

    // Execute the statement.
    SQLite3_CHECK(sqlite3_step(stmt),db);

    // Load the coefficients.
    x_1           = sqlite3_column_double(stmt,0);
    x_L45         = sqlite3_column_double(stmt,1);
    x_L90         = sqlite3_column_double(stmt,2);
    x_L45_2       = sqlite3_column_double(stmt,3);
    x_L45_L90     = sqlite3_column_double(stmt,4);
    x_L90_2       = sqlite3_column_double(stmt,5);
    x_L45_3       = sqlite3_column_double(stmt,6);
    x_L45_2_L90   = sqlite3_column_double(stmt,7);
    x_L45_L90_2   = sqlite3_column_double(stmt,8);
    x_L90_3       = sqlite3_column_double(stmt,9);
    x_L45_4       = sqlite3_column_double(stmt,10);
    x_L45_3_L90   = sqlite3_column_double(stmt,11);
    x_L45_2_L90_2 = sqlite3_column_double(stmt,12);
    x_L45_L90_3   = sqlite3_column_double(stmt,13);
    x_L90_4       = sqlite3_column_double(stmt,14);
    y_1           = sqlite3_column_double(stmt,15);
    y_L45         = sqlite3_column_double(stmt,16);
    y_L90         = sqlite3_column_double(stmt,17);
    y_L45_2       = sqlite3_column_double(stmt,18);
    y_L45_L90     = sqlite3_column_double(stmt,19);
    y_L90_2       = sqlite3_column_double(stmt,20);
    y_L45_3       = sqlite3_column_double(stmt,21);
    y_L45_2_L90   = sqlite3_column_double(stmt,22);
    y_L45_L90_2   = sqlite3_column_double(stmt,23);
    y_L90_3       = sqlite3_column_double(stmt,24);
    y_L45_4       = sqlite3_column_double(stmt,25);
    y_L45_3_L90   = sqlite3_column_double(stmt,26);
    y_L45_2_L90_2 = sqlite3_column_double(stmt,27);
    y_L45_L90_3   = sqlite3_column_double(stmt,28);
    y_L90_4       = sqlite3_column_double(stmt,29);

    // Cleanup the statement.
    SQLite3_CHECK(sqlite3_finalize(stmt),db);

    // Close the database file.
    SQLite3_CHECK(sqlite3_close(db),db);
}

