#include <linux/types.h>
#include <cstdio>

#include <include/lidarlite_v3.h>

LIDARLite_v3 myLidarLite;

int main()
{
    __u16 distance;
    __u8  busyFlag;

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
            // When no longer busy, immediately initialize another measurement
            // and then read the distance data from the last measurement.
            // This method will result in faster I2C rep rates.
            myLidarLite.takeRange();
            distance = myLidarLite.readDistance();

            printf("%4d\n", distance);
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
