/*
* gesture.h
* A library for SeeedStudio gesture recognition
*
* Copyright (c) 2013 seeed technology inc.
* Author        :   lawliet.zou(lawliet.zou@gmail.com)
* Create Time   :   Dec 24, 2013
* Change Log    :
*
* The MIT License (MIT)
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/
#ifndef _GESTURE_H_
#define _GESTURE_H_

#include <Wire.h>
#include <ADXL345.h>

#define DEBUG                       0
#define SAMPLING_NUMBER             200
#define SAMPLING_MODEL_NUMBER       60
#define START_THRESHOLD             40
#define START_THRESHOLD_NUMBER      10
#define END_THRESHOLD               20
#define END_CHECK_NUMBER            10
#define SPEED_THRESHOLD_PERCENT     0.3
#define MODEL_NUMBER                16

/** Gesture class.
 * offer some APIs to support gesture recognition.
 */
class Gesture
{
public:
    /** initialize member variable and 3-Axis Digital Accelerometer
     */
    void init(void);

    /** be ready to recognize gesture
     */

    int getVergence(void);

    int wakeUp(void);

    /** check whether gesture has been started
     *  @returns
     *      0 on success
     *      -1 on error
     */
    int checkMoveStart(void);

    /** obtain accelerate data till end of gesture if gesture has been started
     *  @returns
     *      0 on success
     *      -1 on error
     */
    int getAccelerateData(void);

    /** calculate weight of character according to the array of accelerate
     *  @returns
     *      0 on success
     *      -1 on error
     */
    int calculateAccelerateData(void);


    /** flag of sampling accelerate data.
     */
    int samplingAccelerateData;

    /** flag of calculating accelerate data
     */
    int calculatingAccelerateData;

    /** array to store accelerate data
     */
    int accelerateArray[SAMPLING_NUMBER][2];

    /** sampling number count
     */
    int samplingIndex;

    /** start flag
     */
    int gestureWakeUp;

private:
    /** initialize 3-Axis Digital Accelerometer
     */
    void accelerateInit(void);

    /** check whether adxl is in high speed shake mode
     */
    int checkHighSpeedShake(void);

    /** obtain adjust parameter before start drawing gesture
     */
    void obtainAdjustParameter(int &model_x,int &model_z);

    /** adjust accelerate data
     *  @returns
     *      0 on success
     *      -1 on error
     */
    int adjustAccelerateData(void);

    /** calculate speed data(integral by accelerate data)
     *  @returns
     *      0 on success
     *      -1 on error
     */
    int calculateSpeedData(void);

    /** calculate character's weight
     *  @returns
     *      0 on success
     *      -1 on error
     */
    int calculateWeightValue(void);

    /** calculate matching degree
     *  @returns
     *      0 on success
     *      -1 on error
     */
    int calculateConfidence(void);

    /** corrected accelerate value of X-axis at beginning
     */
    int accelerateStartModel_x;

    /** corrected accelerate value of Z-axis at beginning
     */
    int accelerateStartModel_z;

    /** corrected accelerate value of X-axis at end
     */
    int accelerateEndModel_x;

    /** corrected accelerate value of Z-axis at end
     */
    int accelerateEndModel_z;

    /** adxl is an instance of the ADXL345 library
     */
    ADXL345 adxl;

    /** store X-axis and Z-axis weight value
     */
    int weightValue[2];
};

#endif