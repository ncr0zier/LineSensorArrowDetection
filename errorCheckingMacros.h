// errorCheckingMacros.h
// Free to use without restriction.
// Copyright (C) 2020  Nathan W. Crozier

#ifndef UNTITLED_ERRORCHECKINGMACROS_H
#define UNTITLED_ERRORCHECKINGMACROS_H

#include <sqlite3.h>
#include "hip/hip_runtime.h"

// Macro and function for checking and reporting SQLite3 errors when the success value is SQLITE_OK.
// Use C Preprocessor to convert the command to a string, get the filename and line number.
// Run the command and send the result to the function.
inline int sqlite3ErrorChecker(std::string com, int retVal, std::string filename, int lineNum, sqlite3* db){
    // There are only a few success values.
    if(retVal != SQLITE_OK && retVal != SQLITE_ROW && retVal != SQLITE_DONE) {
        // Print the line filename and command.
        std::cerr << "The below command generated an error.\n";
        std::cerr << "File: " << filename << " Line: " << lineNum << " Command: " << com << std::endl;
        std::cerr << "SQLite3 Error: " << sqlite3_errmsg(db) << std::endl;
        std::cerr << "Error Number: " << retVal << std::endl;
        std::abort();
    }
    return retVal;
}

#define SQLite3_CHECK(command,db) sqlite3ErrorChecker(#command, command, __FILE__, __LINE__,db)

// Inline function and macro for checking and reporting hip errors using the same approach as checking for SQLite3 errors.
inline void hipErrorChecker(std::string com, hipError_t status, std::string filename, int lineNum){
    // These are the only success values.
    if(status != hipSuccess) {
        // Print the line filename and command.
        std::cerr << "The below command generated an error.\n";
        std::cerr << "File: " << filename << " Line: " << lineNum << " Command: " << com << std::endl;
        std::cerr << "Error: HIP reports " << hipGetErrorString(status) << std::endl;
        std::abort();
    }
}

// Based on the HIP_CHECK macro from this presentation: INTRODUCTION TO AMD GPU PROGRAMMING WITH HIP
// https://www.exascaleproject.org/wp-content/uploads/2017/05/ORNL_HIP_webinar_20190606_final.pdf
#define HIP_CHECK(command) hipErrorChecker(#command, command, __FILE__, __LINE__)

#endif //UNTITLED_ERRORCHECKINGMACROS_H
