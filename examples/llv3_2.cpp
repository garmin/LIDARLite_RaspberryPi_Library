/*------------------------------------------------------------------------------
  This example illustrates how to use the LIDAR-Lite library to gain quick
  access to the basic functions of LIDAR-Lite via the Raspberry Pi interface.
  Additionally, it provides users of any platform with a template for their
  own application code.
  
  Copyright (c) 2019 Garmin Ltd. or its subsidiaries.
  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at
  http://www.apache.org/licenses/LICENSE-2.0
  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
------------------------------------------------------------------------------*/

#include <linux/types.h>
#include <cstdio>

#include <include/lidarlite_v3.h>

LIDARLite_v3 myLidarLite;

int main()
{
    __u16 distance;
	__u16 signalstrength;
    __u8  busyFlag;
	//__u32 distance1;
	//__u32 signalstrength1;
	double distance1;
	double signalstrength1;
	double i_avg;
	int i;
	i_avg = 25;
	i = -1;
	distance1 = 0;
	signalstrength1 = 0;

    // Initialize i2c peripheral in the cpu core
    myLidarLite.i2c_init();

    // Optionally configure LIDAR-Lite
    myLidarLite.configure(0);

	
    while(1)
    {
        // Each time through the loop, check BUSY
        busyFlag = myLidarLite.getBusyFlag();

        if (busyFlag == 0x00)
        {
			i = i + 1;
            // When no longer busy, immediately initialize another measurement
            // and then read the distance data from the last measurement.
            // This method will result in faster I2C rep rates.
            myLidarLite.takeRange();
            distance = myLidarLite.readDistance();
			signalstrength = myLidarLite.readsignalstrength();
			//distance1 = distance1 + (double)distance/100;
			//signalstrength1 = signalstrength1 + (double)signalstrength/100;
			//printf("%4d,%4d\n", distance, signalstrength);
			
			distance1 		= distance1 		+ 	((double)distance)/i_avg;
			signalstrength1 = signalstrength1 	+ 	((double)signalstrength)/i_avg;
			//printf("%d,%d\n", distance1, signalstrength1);
			
			if (i>i_avg-1){
				//printf("%d,%d\n", distance1, signalstrength1);
				printf("%f,%f\n", distance1, signalstrength1);
				fflush(stdout);
				i = -1;
				
				distance1 = 0;
				signalstrength1 = 0;
			}
			

            
        }
    }
}


/*
 * ================================================================
 * The set of instructions below illustrates one method of setting
 * an alternate I2C device address in the LIDAR-Lite v3. See the
 * operator manual and C++ libraries for further details.
 * ================================================================
 */

/*
#define i2cSecondaryAddr 0x44  // Set I2C address of LIDAR-Lite v3 to 0x44

int main()
{
    __u16 distance;

    // Initialize i2c peripheral in the cpu core
    myLidarLite.i2c_init();

    // Set an alternate i2c address in the LIDAR-Lite
    // The 2nd argument, if non-zero, disables the default addr 0x62
    myLidarLite.setI2Caddr(i2cSecondaryAddr, true);

    while(1)
    {
        myLidarLite.waitForBusy(i2cSecondaryAddr);
        myLidarLite.takeRange(i2cSecondaryAddr);
        distance      = myLidarLite.readDistance(i2cSecondaryAddr);

        printf("%4d\n", distance);
    }
}
*/
