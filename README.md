# Line Sensor Arrow Detection
<li>A program to create a sensor baseline per pixel calculating the average, minimum, maximum, standard deviation, and 
detection threshold (average minus five times the standard deviation) using several hundred thousand readings. Frames 
are copied to GPU memory as they're collected and calculations performed using the GPU after collection finishes.</li>
<li> A program to collect datapoints correllating pixels being blocked and screen position for training a calibration equation (a fourth-order polynomial)</li>
<li> A python program using scikit-learn's multiple regression algorithm to calculate 30 coefficients needed for the polynomial calibration equation</li>
<li>A program using the calibration equation to estimate the position on the screen of an arrow from sensor data. </li>
Detecting arrows in flight is a work in progress. Data is stored and retrieved using SQLite between programs.


## Usage

<ul>Run the baseline program to generate a baseline for the sensors.</ul>
<ul>Use the training program to get datapoints for calibrating the screen. (The points chosen should weight each part of the screen equally.)</ul>
<uL>Calculate the coefficients for the calibration equation with using multiple linear regression with regression_fitting.py.</uL>
<ul>Test the screen calibration with the testing_continuous program.</ul>

[Here's an example of the screen calibration being tested.](https://www.youtube.com/watch?v=hSHJYvhAOAk)

### Libraries Used

<li>AMD ROCm Package</li> 
<li>SQLite3</li>
<li>SDL2</li>
<li>Basler Pylon 5 </li>
<li>pandas</li>
<li>scikit-learn</li>

### Hardware Used

<li>Two Basler Runner GigE Line Cameras (Model: ruL1024-57gm)</li> 
<li>A strip of 850nm LED lights is used as a light source for the cameras.</li>
<li>Intel Ethernet Server Adapter I340-T4 </li>
<li>An AMD Vega 64 is used to process camera frames.</li>
<li>The camera mounts and kevlar screen are from an Interactive Target Systems archery range.</li>


## Authors

* **Nathan Crozier** - *Initial work*

## License

This project is licensed under the GPLv3+ - see the [LICENSE.md](LICENSE.md) file for details


## Acknowledgments

* [Introduction to AMD GPU programming with HIP Webinar - June 7, 2019](https://www.youtube.com/watch?v=3ZXbRJVvgJs) was invaluable for writing the GPGPU functions. 
* Basler's sample programs and API manual were used as a starting point for getting data off the cameras.
* My parents for getting a archery range using line cameras to detect arrows in the late 90s.

