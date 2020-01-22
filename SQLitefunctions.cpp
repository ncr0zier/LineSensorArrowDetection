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

#include "SQLitefunctions.h"

void writeDataPointsToDB(std::vector<struct DataPoint> & data, const char * filename){
    // Open the database file.
    sqlite3 *db;
    SQLite3_CHECK(sqlite3_open(filename, &db),db);

    // Get the current datetime.
    sqlite3_stmt * stmt;
    SQLite3_CHECK(sqlite3_prepare_v2(db,"SELECT datetime('now','localtime');",-1,&stmt,NULL),db);
    SQLite3_CHECK(sqlite3_step(stmt),db);
    std::string currentDatetime = reinterpret_cast<const char*>(sqlite3_column_text   (stmt,0));
    SQLite3_CHECK(sqlite3_finalize(stmt),db);

    // Create the table for training datapoints.
    SQLite3_CHECK(sqlite3_prepare_v2(db,CREATE_TRAINING_TABLE_QUERY,-1,&stmt,NULL),db);
    SQLite3_CHECK(sqlite3_step(stmt),db);
    SQLite3_CHECK(sqlite3_finalize(stmt),db);

    // Doing multiple inserts inside of a transaction is faster.
    // https://stackoverflow.com/questions/1711631/improve-insert-per-second-performance-of-sqlite
    SQLite3_CHECK(sqlite3_prepare_v2(db,"BEGIN TRANSACTION",-1,&stmt,NULL),db);
    SQLite3_CHECK(sqlite3_step(stmt),db);
    SQLite3_CHECK(sqlite3_finalize(stmt),db);

    // Prepare the statement for writing datapoints to the db.
    SQLite3_CHECK(sqlite3_prepare_v2(db,INSERT_TRAINING_TABLE_QUERY,-1,&stmt,NULL),db);
    for(int i = 0; i < data.size(); i++){
        // Bind variables.
        SQLite3_CHECK(sqlite3_bind_int(stmt,1,data[i].x),db);
        SQLite3_CHECK(sqlite3_bind_int(stmt,2,data[i].y),db);
        SQLite3_CHECK(sqlite3_bind_double(stmt,3,data[i].L45),db);
        SQLite3_CHECK(sqlite3_bind_double(stmt,4,data[i].L90),db);
        SQLite3_CHECK(sqlite3_bind_text  (stmt,10,currentDatetime.c_str(),-1,NULL),db);

        // Execute Statement.
        SQLite3_CHECK(sqlite3_step(stmt),db);

        // Reset Statement to use again.
        SQLite3_CHECK(sqlite3_reset(stmt),db);
    }
    // Finalize the statement.
    SQLite3_CHECK(sqlite3_finalize(stmt),db);

    // End the transaction.
    SQLite3_CHECK(sqlite3_prepare_v2(db,"END TRANSACTION",-1,&stmt,NULL),db);
    SQLite3_CHECK(sqlite3_step(stmt),db);
    SQLite3_CHECK(sqlite3_finalize(stmt),db);

    // Close the database file.
    SQLite3_CHECK(sqlite3_close(db),db);
}