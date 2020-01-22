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

#ifndef UNTITLED_SQLITEFUNCTIONS_H
#define UNTITLED_SQLITEFUNCTIONS_H

// Include files for SQLite
#include <sqlite3.h>
#include "main_training.h"

// SQL Queries
#define CREATE_TRAINING_TABLE_QUERY "CREATE TABLE IF NOT EXISTS 'trainingData' ('x' INTEGER, 'y' INTEGER, 'L45' REAL, 'L90' REAL, 'timeCreated' TEXT);"
#define INSERT_TRAINING_TABLE_QUERY "INSERT INTO  'trainingData' VALUES (?,?,?,?,?);"

// Function Definition for writing data points to an SQLite file.
void writeDataPointsToDB(std::vector<struct DataPoint> & data, const char * filename);

#endif //UNTITLED_SQLITEFUNCTIONS_H
