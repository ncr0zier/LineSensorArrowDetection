# // Line Sensor Arrow Detection uses line sensors to measure the location an
# // arrow hits a projector screen.
# //
# // Copyright (C) 2020  Nathan W. Crozier
# //
# // This file is part of Line Sensor Arrow Detection
# //
# // Line Sensor Arrow Detection is free software: you can redistribute it and/or modify
# // it under the terms of the GNU General Public License as published by
# // the Free Software Foundation, either version 3 of the License, or
# // (at your option) any later version.
# //
# // Line Camera Arrow Detection is distributed in the hope that it will be useful,
# // but WITHOUT ANY WARRANTY; without even the implied warranty of
# // MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# // GNU General Public License for more details.
# 	//
# // You should have received a copy of the GNU General Public License
# // along with Line Camera Arrow Detection.  If not, see <https://www.gnu.org/licenses/>.

from sklearn.preprocessing import PolynomialFeatures
from sklearn import linear_model
import numpy as np
from numpy import array
import pandas as pd
import sqlite3

database_filename = 'training_new.db'

# Function returns the current time from an SQL query.
def getCurrentDatetime(conn):
	c = conn.cursor()
	c.execute("SELECT datetime('now','localtime');");
	unprocessed_data = c.fetchall()
	return unprocessed_data[0][0]

## SQL Statements. ##

# Reading all times traning data has been created.
SELECT_TRAINING_TIMES_QUERY = "SELECT DISTINCT timeCreated FROM 'trainingData' ORDER BY timeCreated DESC;"

# Reading Training Data
SELECT_TRAINING_QUERY = "SELECT * FROM 'trainingData' WHERE timeCreated=?;"

# Writing calculated coefficients.
# Column Names match variable names in the C++ program.
CREATE_TABLE_COEF_QUERY = '''CREATE TABLE IF NOT EXISTS coefficients (
 x_1  REAL, x_L45  REAL, x_L90  REAL, x_L45_2  REAL, x_L45_L90  REAL, x_L90_2  REAL, x_L45_3  REAL, x_L45_2_L90  REAL, x_L45_L90_2  REAL, x_L90_3  REAL, x_L45_4  REAL, x_L45_3_L90  REAL, x_L45_2_L90_2  REAL, x_L45_L90_3  REAL, x_L90_4  REAL,
 y_1  REAL, y_L45  REAL, y_L90  REAL, y_L45_2  REAL, y_L45_L90  REAL, y_L90_2  REAL, y_L45_3  REAL, y_L45_2_L90  REAL, y_L45_L90_2  REAL, y_L90_3  REAL, y_L45_4  REAL, y_L45_3_L90  REAL, y_L45_2_L90_2  REAL, y_L45_L90_3  REAL, y_L90_4  REAL,
 'timeCreated' TEXT );'''

INSERT_COEF_QUERY = "INSERT INTO coefficients VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);"
#####################

# Open the database and create a cursor.
conn = sqlite3.connect(database_filename)
c = conn.cursor()

# Get all times training data has been written to the database.
c.execute(SELECT_TRAINING_TIMES_QUERY)
unprocessed_data = c.fetchall()

# Get the most recent time.
most_recent_time = unprocessed_data[0][0]

# Get all rows for training datapoints collected.
c.execute(SELECT_TRAINING_QUERY,[most_recent_time])
unprocessed_data = c.fetchall()

# Lists used for reading in data.
x_list   = []
y_list   = []
L45_list = []
L90_list = []

# Read the datapoints into a list.
for entry in unprocessed_data:
	x_list.append(entry[0])
	y_list.append(entry[1])
	L45_list.append(entry[2])
	L90_list.append(entry[3])

# Convert the lists to numpy arrays to use with pandas.
train_x = np.asarray(x_list)
train_y = np.asarray(y_list)
train_L45 = np.asarray(L45_list)
train_L90 = np.asarray(L90_list)

# Create a dataframe with all the coefficients for a fourth order polynomial.
CameraData = pd.DataFrame({'L45': train_L45, 'L90': train_L90})
p = PolynomialFeatures(degree=4)
CameraDataProcessed = pd.DataFrame(p.fit_transform(CameraData), columns=p.get_feature_names(CameraData.columns))

# Create a dataframe for screen coordinates.
XY = pd.DataFrame({'X': train_x, 'Y': train_y})

# Fit the data to a polynomial.
lm = linear_model.LinearRegression(fit_intercept=False)
model = lm.fit(CameraDataProcessed,XY)

# Generate coefficients for inserting and the current time must be last.
coef_for_sql_insert = list(model.coef_[0]) + list(model.coef_[1])
current_time = getCurrentDatetime(conn)
coef_for_sql_insert.append(current_time)

# Create the table if it doesn't exist and insert the coefficients.
c.execute(CREATE_TABLE_COEF_QUERY)
c.execute(INSERT_COEF_QUERY,coef_for_sql_insert)
conn.commit()

# Report the timeCreated columns.
print("Coefficients have been generated for the training data collected: " + most_recent_time)
print("Row in coefficients timeCreated column: " + current_time)