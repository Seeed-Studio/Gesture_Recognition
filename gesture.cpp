
/*
* gesture.cpp
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
#include "gesture.h"
/*********************************************************************************************************************************************
*|   0   |   1   |   2  |   3   |    4   |    5   |    6   |    7  |    8  |    9    |    10   |   11  |   12  |   13    |    14   |   15  |*
*|   *   |   *   |  *   |   *   |  * * * |      * |* * *   |*      | *     |* * * *  |  * * * *|    *  |*      |  *      |      *  |      *|*
*|  ***  |   *   | *    |    *  | * *    |     *  |   * *  | *     |* * * *|      *  |  *      |* * * *|*      |* * *    |    * * *|      *|*
*| * * * | * * * |******|****** |*   *   |*   *   |  *   * |  *   *| *    *|      *  |  *      |*   *  |*   *  |  *      |      *  |  *   *|*
*|   *   |  ***  | *    |    *  |     *  | * *    | *      |   * * |      *|    * * *|* * *    |*      |* * * *|  *      |      *  |* * * *|*
*|   *   |   *   |  *   |   *   |      * |  * * * |*       |* * *  |      *|      *  |  *      |*      |    *  |  * * * *|* * * *  |  *    |*
*********************************************************************************************************************************************
*/
int weightModel[MODEL_NUMBER][2] = {{128,192},{128,64},{64,128},{192,128},{64,192},{64,64},{192,192},{192,64},{144,224},{224,144},{96,144},{176,224},{176,96},{96,176},{224,176},{144,96}};

void Gesture::init(void)
{
    accelerateStartModel_x = 0;
    accelerateStartModel_z = 0;
    accelerateEndModel_x = 0;
    accelerateEndModel_z = 0;
    samplingIndex = 0;
    samplingAccelerateData = 0;
    calculatingAccelerateData = 0;
    accelerateInit();
}

void Gesture::accelerateInit(void)
{
    adxl.powerOn();

//set activity/ inactivity thresholds (0-255)
    adxl.setActivityThreshold(75); //62.5mg per increment
    adxl.setInactivityThreshold(75); //62.5mg per increment
    adxl.setTimeInactivity(10); // how many seconds of no activity is inactive?

//look of activity movement on this axes - 1 == on; 0 == off
    adxl.setActivityX(1);
    adxl.setActivityY(1);
    adxl.setActivityZ(1);

//look of inactivity movement on this axes - 1 == on; 0 == off
    adxl.setInactivityX(1);
    adxl.setInactivityY(1);
    adxl.setInactivityZ(1);

//look of tap movement on this axes - 1 == on; 0 == off
    adxl.setTapDetectionOnX(0);
    adxl.setTapDetectionOnY(0);
    adxl.setTapDetectionOnZ(1);

//set values for what is a tap, and what is a double tap (0-255)
    adxl.setTapThreshold(50); //62.5mg per increment
    adxl.setTapDuration(15); //625us per increment
    adxl.setDoubleTapLatency(80); //1.25ms per increment
    adxl.setDoubleTapWindow(200); //1.25ms per increment

//set values for what is considered freefall (0-255)
    adxl.setFreeFallThreshold(7); //(5 - 9) recommended - 62.5mg per increment
    adxl.setFreeFallDuration(45); //(20 - 70) recommended - 5ms per increment

//setting all interrupts to take place on int pin 1
//I had issues with int pin 2, was unable to reset it
    adxl.setInterruptMapping( ADXL345_INT_SINGLE_TAP_BIT,   ADXL345_INT1_PIN );
    adxl.setInterruptMapping( ADXL345_INT_DOUBLE_TAP_BIT,   ADXL345_INT1_PIN );
    adxl.setInterruptMapping( ADXL345_INT_FREE_FALL_BIT,    ADXL345_INT1_PIN );
    adxl.setInterruptMapping( ADXL345_INT_ACTIVITY_BIT,     ADXL345_INT1_PIN );
    adxl.setInterruptMapping( ADXL345_INT_INACTIVITY_BIT,   ADXL345_INT1_PIN );

//register interrupt actions - 1 == on; 0 == off
    adxl.setInterrupt( ADXL345_INT_SINGLE_TAP_BIT, 1);
    adxl.setInterrupt( ADXL345_INT_DOUBLE_TAP_BIT, 1);
    adxl.setInterrupt( ADXL345_INT_FREE_FALL_BIT,  1);
    adxl.setInterrupt( ADXL345_INT_ACTIVITY_BIT,   1);
    adxl.setInterrupt( ADXL345_INT_INACTIVITY_BIT, 1);
}


int Gesture::getVergence(void)
{
    int x,y,z;
    int count0 = 0,count1 = 0,count2 = 0;
    while(1) {
        adxl.readXYZ(&x, &y, &z);
        if((abs(x) > 100) || (abs(y) > 100)) {
            if((x<-100)&&(abs(y)<70)) {
                if(++count0>10) {
                    break;
                }
            } else if((y>100)&&(abs(x)<70)) {
                if(++count1>10) {
                    break;
                }
            } else if((y<-100)&&(abs(x)<70)) {
                if(++count2>10) {
                    break;
                }
            }
        }
        delay(50);
    }
    if(++count0>10) {
        return 0;
    } else if(++count1>10) {
        return 1;
    } else if(++count2>10) {
        return 2;
    }
}

int Gesture::checkHighSpeedShake(void)
{
    int tmp[10][3];
    int highSpeedShakeCount = 0;
    for(int i = 0; i < 10; i ++) {
        adxl.readXYZ(&tmp[i][0], &tmp[i][1], &tmp[i][2]);
        if((abs(tmp[i][0]) > 400) || (abs(tmp[i][1]) > 400) || (abs(tmp[i][2]) > 400)) {
            highSpeedShakeCount += 1;
        }
    }
    return highSpeedShakeCount;
}

int Gesture::wakeUp(void)
{
    int x, y, z;
    int notMoveStartDetect = 0;
    int notMoveStartCount = 0;
    adxl.readXYZ(&y, &x, &z);
//Serial.print("\r\n test");
    if((abs(x) > 500) || (abs(y) > 500) || (abs(z) > 500)) {
        unsigned long timerStart,timerEnd;
        timerStart = millis();
        while(1) {
            int highSpeedShakeCount = checkHighSpeedShake();
#if DEBUG
            Serial.print("\r\nhighSpeedCount = ");
            Serial.print(highSpeedShakeCount);
#endif
            if(highSpeedShakeCount < 2) {
                if(!notMoveStartDetect) {
                    notMoveStartDetect = 1;
                }
                notMoveStartCount++;
            } else {
                if(notMoveStartDetect) {
                    notMoveStartDetect = 0;
                    notMoveStartCount = 0;
                }
            }
            if(notMoveStartDetect) {
                if(notMoveStartCount >= 2) {
                    gestureWakeUp = 0;
                    notMoveStartDetect = 0;
                    break;
                }
            }
            timerEnd = millis();
            if(timerEnd - timerStart > 1000) {
                gestureWakeUp = 1;
                break;
            }
            delay(100);
        }
    }
    return 0;
}

int Gesture::checkMoveStart(void)
{
    int x,y,z;
    int thresholdCount = 0;
    obtainAdjustParameter(accelerateStartModel_x,accelerateStartModel_z);
#if DEBUG
    Serial.print("\r\nAdjustParameter is :");
    Serial.print("\r\n X = ");
    Serial.print(accelerateStartModel_x);
    Serial.print("\r\n Z = ");
    Serial.println(accelerateStartModel_z);
#endif
    while(1) {
        adxl.readXYZ(&x, &y, &z);
        if(abs(x-accelerateStartModel_x) + abs(z-accelerateStartModel_z) > START_THRESHOLD) {
            thresholdCount++;
            if(thresholdCount > START_THRESHOLD_NUMBER) {
                Serial.print("\r\n start!!!");
                samplingIndex = 0;
                accelerateArray[samplingIndex][0] = x - accelerateStartModel_x;
                accelerateArray[samplingIndex][1] = z - accelerateStartModel_z;
                samplingAccelerateData = 1;
                break;
            }
        } else {
            thresholdCount = 0;
        }
    }
    return 0;
}

void Gesture::obtainAdjustParameter(int &model_x,int &model_z)
{
    int x,y,z;
    model_x = 0;
    model_z = 0;
    for(int i = 0; i < SAMPLING_MODEL_NUMBER; i++) {
        adxl.readXYZ(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z
        model_x += x;
        model_z += z;
    }
    model_x /= SAMPLING_MODEL_NUMBER;
    model_z /= SAMPLING_MODEL_NUMBER;
}

int Gesture::getAccelerateData()
{
    int x,y,z;
    int moveEndCheckNumber = 0;
    while(1) {
        adxl.readXYZ(&x, &y, &z);
        if(++samplingIndex >= SAMPLING_NUMBER) {
            return -1;
        }
        accelerateArray[samplingIndex][0] = x - accelerateStartModel_x;
        accelerateArray[samplingIndex][1] = z - accelerateStartModel_z;
        delay(10); //ToDo:Define sampling rate here!

        if(abs(accelerateArray[samplingIndex][0] - accelerateArray[samplingIndex-1][0]) +
                abs(accelerateArray[samplingIndex][1] - accelerateArray[samplingIndex-1][1]) < END_THRESHOLD) {
            moveEndCheckNumber++;
            delay(5);
            accelerateEndModel_x += accelerateArray[samplingIndex][0];
            accelerateEndModel_z += accelerateArray[samplingIndex][1];

            if(moveEndCheckNumber > END_CHECK_NUMBER) {
                samplingAccelerateData = 0;
                calculatingAccelerateData = 1;
                samplingIndex -= END_CHECK_NUMBER;
                accelerateEndModel_x /= END_CHECK_NUMBER;
                accelerateEndModel_z /= END_CHECK_NUMBER;
                samplingIndex -= 10; //ToDo: need to check
                break;
            }
        } else {
            moveEndCheckNumber = 0;
            accelerateEndModel_x = 0;
            accelerateEndModel_z = 0;
        }
    }
#if DEBUG
    Serial.print("\r\n accelerateEndModel of X,Z = : ");
    Serial.print("\t");
    Serial.print(accelerateEndModel_x);
    Serial.print("\t");
    Serial.println(accelerateEndModel_z);
#endif
//adjust accelerate
    for(int index = samplingIndex - 1; index > samplingIndex/2; index--) {
        accelerateArray[index][0] -= accelerateEndModel_x*index/samplingIndex;
        accelerateArray[index][1] -= accelerateEndModel_z*index/samplingIndex;
    }
    return 0;
}

int Gesture::calculateAccelerateData()
{
    adjustAccelerateData();
    calculateSpeedData();
    calculatingAccelerateData = 0;
    calculateWeightValue();
#if DEBUG
    Serial.print("\r\nweightValue_x = ");
    Serial.print(weightValue[0]);
    Serial.print("\r\nweightValue_z = ");
    Serial.println(weightValue[1]);
#endif
    calculateConfidence();
    return 0;
}

int Gesture::adjustAccelerateData()
{
    float accelerate_x;
    float accelerate_z;
    float accelerate_ratio;
    int accelerate_average = 0;
    int maxAccelerate = 0;
    int validIndex = 0;

    for(int accelerateIndex = 1; accelerateIndex < samplingIndex; accelerateIndex++) {
        maxAccelerate = max(maxAccelerate,max(abs(accelerateArray[accelerateIndex][0]),abs(accelerateArray[accelerateIndex][1])));
    }
    int adjustAccelerateValue = maxAccelerate/10;
    int adjustAccelerate_x = 0;
    int adjustAccelerate_z = 0;

    for(int accelerateIndex = 0; accelerateIndex < samplingIndex; accelerateIndex++) {
        adjustAccelerate_x = accelerateArray[accelerateIndex][0]*10/adjustAccelerateValue;
        adjustAccelerate_z = accelerateArray[accelerateIndex][1]*10/adjustAccelerateValue;
        accelerateArray[accelerateIndex][0] = abs(adjustAccelerate_x) < 15?0:adjustAccelerate_x;
        accelerateArray[accelerateIndex][1] = abs(adjustAccelerate_z) < 15?0:adjustAccelerate_z;
#if DEBUG
        Serial.print("\r\n normalize accelerate of X,Z = : ");
        Serial.print("\t");
        Serial.print(accelerateArray[accelerateIndex][0]);
        Serial.print("\t");
        Serial.println(accelerateArray[accelerateIndex][1]);
#endif
    }

    for(int index  = 0; index < samplingIndex; index++) {
        if((accelerateArray[index][0] != 0) || (accelerateArray[index][1] != 0)) {
            accelerateArray[validIndex][0] = accelerateArray[index][0];
            accelerateArray[validIndex][1] = accelerateArray[index][1];
            validIndex++;
        }
    }
    samplingIndex = validIndex;

    for(int accelerateIndex = 1; accelerateIndex < samplingIndex; accelerateIndex++) {
        accelerate_x = abs(accelerateArray[accelerateIndex][0]);
        accelerate_z = abs(accelerateArray[accelerateIndex][1])==0?1:abs(accelerateArray[accelerateIndex][1]);
        accelerate_ratio = accelerate_x/accelerate_z;
        if(accelerate_x/accelerate_z > 3) {
            accelerateArray[accelerateIndex][1] = 0;
            if((accelerateIndex > 3) && ((accelerateArray[accelerateIndex-2][1] < 20) || (accelerateArray[accelerateIndex-3][1] < 20))) {
                accelerateArray[accelerateIndex-1][1] = 0;
                accelerateArray[accelerateIndex-2][1] = 0;
            }
        } else if((accelerate_x/accelerate_z < 1.7) && (accelerate_x/accelerate_z > 0.6)) {
            accelerate_average = (accelerate_x+accelerate_z)/2;
            accelerateArray[accelerateIndex][0] = accelerateArray[accelerateIndex][0]>0?accelerate_average:-accelerate_average;
            accelerateArray[accelerateIndex][1] = accelerateArray[accelerateIndex][1]>0?accelerate_average:-accelerate_average;
        } else if(accelerate_x/accelerate_z < 0.33) { //15
            accelerateArray[accelerateIndex][0] = 0;
            if((accelerateIndex > 3) && ((accelerateArray[accelerateIndex-2][0] < 20) || (accelerateArray[accelerateIndex-3][0] < 20))) {
                accelerateArray[accelerateIndex-1][0] = 0;
                accelerateArray[accelerateIndex-2][0] = 0;
            }
        }
        if(accelerateIndex > 3) {
            if((accelerateArray[accelerateIndex][0] == 0) && ((accelerateArray[accelerateIndex-3][0] == 0))) {
                for(int i = accelerateIndex; i > accelerateIndex-3; i--) {
                    accelerateArray[i][0] = 0;
                }
            }
            if((accelerateArray[accelerateIndex][1] == 0) && ((accelerateArray[accelerateIndex-3][1] == 0))) {
                for(int i = accelerateIndex; i > accelerateIndex-3; i--) {
                    accelerateArray[i][1] = 0;
                }
            }
        }
    }

    validIndex = 0;
    for(int index  = 0; index < samplingIndex; index++) {
        if((accelerateArray[index][0] != 0) || (accelerateArray[index][1] != 0)) {
            accelerateArray[validIndex][0] = accelerateArray[index][0];
            accelerateArray[validIndex][1] = accelerateArray[index][1];
            validIndex++;
        }
    }
    samplingIndex = validIndex;

#if DEBUG
    for(int index = 0; index < samplingIndex; index++) {
        Serial.print("\r\n accelerate of X,Z = : ");
        Serial.print("\t");
        Serial.print(accelerateArray[index][0]);
        Serial.print("\t");
        Serial.println(accelerateArray[index][1]);
    }
#endif

    return 0;
}

int Gesture::calculateSpeedData()
{
    int maxSpeed = 0;
    for(int speedIndex = 1; speedIndex < samplingIndex; speedIndex++) {
        if(abs(accelerateArray[speedIndex][0]) +  abs(accelerateArray[speedIndex+3][0]) + abs(accelerateArray[speedIndex+5][0]) == 0) {
            accelerateArray[speedIndex][0] = 0;
        } else {
            accelerateArray[speedIndex][0] += accelerateArray[speedIndex-1][0];
        }
        if(abs(accelerateArray[speedIndex][1]) + abs(accelerateArray[speedIndex+3][1]) + abs(accelerateArray[speedIndex+5][1]) == 0) {
            accelerateArray[speedIndex][1] = 0;
        } else {
            accelerateArray[speedIndex][1] += accelerateArray[speedIndex-1][1];
        }

        if((accelerateArray[speedIndex][0] == 0)&&(abs(accelerateArray[speedIndex-1][0]) > 100)) {
            int damping = 0;
            while(++damping < 15) {
                if(abs(accelerateArray[speedIndex - damping][0]) < 100) {
                    for(int i = 0; i < damping; i++) {
                        accelerateArray[speedIndex-i][0] = 0;
                    }
                    break;
                }
            }
        }
        if((accelerateArray[speedIndex][1] == 0)&&(abs(accelerateArray[speedIndex-1][1]) > 100)) {
            int damping = 0;
            while(++damping < 15) {
                if(abs(accelerateArray[speedIndex-damping][1]) < 100) {
                    for(int i = 0; i < damping; i++) {
                        accelerateArray[speedIndex-i][1] = 0;
                    }
                    break;
                }
            }
        }

    }

#if DEBUG
    for(int speedIndex = 1; speedIndex < samplingIndex; speedIndex++) {
        Serial.print("\r\n Speed of X,Z = : ");
        Serial.print("\t");
        Serial.print(accelerateArray[speedIndex][0]);
        Serial.print("\t");
        Serial.println(accelerateArray[speedIndex][1]);
    }
#endif

    for(int speedIndex = 1; speedIndex < samplingIndex; speedIndex++) {
        maxSpeed = max(maxSpeed,max(abs(accelerateArray[speedIndex][0]),abs(accelerateArray[speedIndex][1])));
    }

    int adjustSpeedValue = (float)maxSpeed*SPEED_THRESHOLD_PERCENT;
    for(int colIndex = 0; colIndex < 2; colIndex++) {
        for(int rowIndex = 0; rowIndex < samplingIndex; rowIndex++) {
            if(accelerateArray[rowIndex][colIndex] < -adjustSpeedValue) {
                accelerateArray[rowIndex][colIndex] = 1;
            } else if(accelerateArray[rowIndex][colIndex] > adjustSpeedValue) {
                accelerateArray[rowIndex][colIndex] = 3;
            } else {
                accelerateArray[rowIndex][colIndex] = 2;
            }
        }
    }
    return 0;
}

int pow_int(int base, int ext)
{
    int value = 1;
    for(int i = 0; i < ext; i++) {
        value *= base;
    }
    return value;
}

int Gesture::calculateWeightValue(void)
{
    int validIndex = 0;
    for(int index  = 0; index < samplingIndex; index++) {
        if((accelerateArray[index][0] != 2) || (accelerateArray[index][1] != 2)) {
            accelerateArray[validIndex][0] = accelerateArray[index][0];
            accelerateArray[validIndex][1] = accelerateArray[index][1];
            validIndex++;
        }
    }
//important
    accelerateArray[validIndex][0] = 0;
    accelerateArray[validIndex][1] = 0;
    validIndex++;
#if DEBUG
    for(int index = 0; index < validIndex; index++) {
        Serial.print("\r\n calculate Speed of X,Z = : ");
        Serial.print("\t");
        Serial.print(accelerateArray[index][0]);
        Serial.print("\t");
        Serial.println(accelerateArray[index][1]);
    }
#endif
    weightValue[0] = 0;
    weightValue[1] = 0;
    for(int row = 0; row < 2; row++) {
        int weight_value = 0;
        int weight_count = 0;
        int weight_old = 0;
        int bitCount = 3;
        int weight_length = (validIndex)/6;
        for(int index = 0; index < validIndex; index++) {
            int tmp = accelerateArray[index][row];
            if(tmp == weight_value) {
                weight_count++;
            } else {
                if((weight_count > weight_length) && (weight_value != weight_old)) {
                    int temp_value = weight_value*pow_int(4,bitCount--);
                    weightValue[row] += temp_value;
                    weight_old = weight_value;
                    weight_length = weight_count*2/5 > weight_length?weight_count*2/5:weight_length;
                }
                weight_count = 0;
                weight_value = tmp;
            }
        }
    }
    return 0;
}

int Gesture::calculateConfidence(void)
{
    int perfectCorrect = 0;
    for(int i = 0; i < MODEL_NUMBER; i++) {
        if((weightValue[0] == weightModel[i][0]) && (weightValue[1] == weightModel[i][1])) {
            Serial.print("\r\nNumber is:");
            Serial.println(i);
            Serial.print("Confidence: 100%");
            perfectCorrect = 1;
            break;
        }
    }
    if(!perfectCorrect) {
        for(int i = 0; i < MODEL_NUMBER; i++) {
            int deviation_x = abs(weightValue[0] - weightModel[i][0]);
            int deviation_y = abs(weightValue[1] - weightModel[i][1]);
            if(((deviation_x <= 48) && (deviation_x >= 0) && (deviation_y == 0)) || ((deviation_y <= 48)&&(deviation_y >= 0)&&(deviation_x == 0))) {
                Serial.print("\r\n Number maybe is: ");
                Serial.println(i);
                Serial.print("Confidence: ");
                Serial.println(1- ((float)deviation_x/(2*weightModel[i][0]) + (float)deviation_y/(2*weightModel[i][1])));
            }
        }
    }
    return 0;
}
