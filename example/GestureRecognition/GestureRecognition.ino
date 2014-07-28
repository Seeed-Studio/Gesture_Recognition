/*****************************************************************************/
//Introduction:this sketch can be used to test gesture recognition.3-Axis Digital 
//   Accelerometer(16g) is in need. Start to test, you should press the button and
//   draw your graphics in the air.Matching results will be seen in serial output.
//   Any problems, please feel free to contact me !
//   now we only support 15 kinds of gestures. It is as follow
//
/*********************************************************************************************************************************************
 *|   0   |   1   |   2	 |   3   |    4   |	5   |	6    |    7  |    8  |    9    |    10   |   11  |   12  |   13    |    14   |   15  |*
 *|   *   |   *   |  *   |   *   |  * * * |	  * |* * *   |*      | *     |* * * *  |  * * * *|    *  |*      |  *      |      *  |      *|*
 *|  ***  |   *   | *    |    *  | * *	  |	 *  |   * *  | *     |* * * *|      *  |  *      |* * * *|*      |* * *    |    * * *|      *|*
 *| * * * | * * * |******|****** |*   *	  | *   *   |  *   * |  *   *| *    *|      *  |  *      |*   *  |*   *  |  *      |      *  |  *   *|*
 *|   *   |  ***  | *    |    *  |     *  |  * *    | *      |   * * |      *|    * * *|* * *    |*      |* * * *|  *      |      *  |* * * *|*
 *|   *   |   *   |  *   |   *   |      * |   * * * |*       |* * *  |      *|      *  |  *      |*      |    *  |  * * * *|* * * *  |  *    |*
 *********************************************************************************************************************************************
//
//  Hardware:3-Axis Digital Accelerometer(16g)
//  Arduino IDE: Arduino-1.0
//  Author:lawliet.zou(lawliet.zou@gmail.com)		
//  Date: Dec 24,2013
//  Version: v1.0
//  by www.seeedstudio.com
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
/*******************************************************************************/
#include <gesture.h>
#include <Wire.h>
#include <ADXL345.h>
int button = 5;
Gesture gesture;
void setup(){
    Serial.begin(9600);
    pinMode(button,INPUT);
    gesture.init();
}

void loop(){
    if(HIGH == digitalRead(button)){
	delay(200);//debug
	if(!gesture.samplingAccelerateData){       
	    gesture.checkMoveStart();
        }
	if(gesture.samplingAccelerateData){
            if(0 != gesture.getAccelerateData()){
                Serial.print("\r\n get accelerate data error.");
            }
	}
	if(gesture.calculatingAccelerateData){
	    gesture.calculateAccelerateData();
        }
    }
}
