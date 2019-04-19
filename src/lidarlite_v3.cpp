/*------------------------------------------------------------------------------
  LIDARLite_v3 Raspberry Pi Library

  This library provides quick access to the basic functions of LIDAR-Lite
  via the Raspberry Pi interface. Additionally, it can provide a user of any
  platform with a template for their own application code.
  
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
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

#include <include/lidarlite_v3.h>

/*------------------------------------------------------------------------------
  I2C Init
  Initialize the I2C peripheral in the processor core
------------------------------------------------------------------------------*/
__s32 LIDARLite_v3::i2c_init (void)
{
    char *filename = (char*)"/dev/i2c-1";

    if ((file_i2c = open(filename, O_RDWR)) < 0)
    {
        //ERROR HANDLING: you can check errno to see what went wrong
        printf("Failed to open the i2c bus");
        return -1;
    }
    else
    {
        return 0;
    }
}

/*------------------------------------------------------------------------------
  I2C Connect
  Connect to the I2C device with the specified device address

  Parameters
  ------------------------------------------------------------------------------
  lidarliteAddress: Default 0x62. Fill in new address here if changed. See
    operating manual for instructions.
------------------------------------------------------------------------------*/
__s32 LIDARLite_v3::i2c_connect (__u8 lidarliteAddress)
{
    if (ioctl(file_i2c, I2C_SLAVE, lidarliteAddress) < 0)
    {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        //ERROR HANDLING; you can check errno to see what went wrong
        return -1;
    }
    else
    {
        return 0;
    }
}

/*------------------------------------------------------------------------------
  Configure
  Selects one of several preset configurations.

  Parameters
  ------------------------------------------------------------------------------
  configuration:  Default 0.
    0: Default mode, balanced performance.
    1: Short range, high speed. Uses 0x1d maximum acquisition count.
    2: Default range, higher speed short range. Turns on quick termination
        detection for faster measurements at short range (with decreased
        accuracy)
    3: Maximum range. Uses 0xff maximum acquisition count.
    4: High sensitivity detection. Overrides default valid measurement detection
        algorithm, and uses a threshold value for high sensitivity and noise.
    5: Low sensitivity detection. Overrides default valid measurement detection
        algorithm, and uses a threshold value for low sensitivity and noise.
  lidarliteAddress: Default 0x62. Fill in new address here if changed. See
    operating manual for instructions.
------------------------------------------------------------------------------*/
void LIDARLite_v3::configure(__u8 configuration, __u8 lidarliteAddress)
{
    __u8 sigCountMax;
    __u8 acqConfigReg;
    __u8 refCountMax;
    __u8 thresholdBypass;

    switch (configuration)
    {
        case 0: // Default mode, balanced performance
            sigCountMax     = 0x80; // Default
            acqConfigReg    = 0x08; // Default
            refCountMax     = 0x05; // Default
            thresholdBypass = 0x00; // Default
            break;

        case 1: // Short range, high speed
            sigCountMax     = 0x1d;
            acqConfigReg    = 0x08; // Default
            refCountMax     = 0x03;
            thresholdBypass = 0x00; // Default
            break;

        case 2: // Default range, higher speed short range
            sigCountMax     = 0x80; // Default
            acqConfigReg    = 0x00;
            refCountMax     = 0x03;
            thresholdBypass = 0x00; // Default
            break;

        case 3: // Maximum range
            sigCountMax     = 0xff;
            acqConfigReg    = 0x08; // Default
            refCountMax     = 0x05; // Default
            thresholdBypass = 0x00; // Default
            break;

        case 4: // High sensitivity detection, high erroneous measurements
            sigCountMax     = 0x80; // Default
            acqConfigReg    = 0x08; // Default
            refCountMax     = 0x05; // Default
            thresholdBypass = 0x80;
            break;

        case 5: // Low sensitivity detection, low erroneous measurements
            sigCountMax     = 0x80; // Default
            acqConfigReg    = 0x08; // Default
            refCountMax     = 0x05; // Default
            thresholdBypass = 0xb0;
            break;

        case 6: // Short range, high speed, higher error
            sigCountMax     = 0x04;
            acqConfigReg    = 0x01; // turn off short_sig, mode pin = status output mode
            refCountMax     = 0x03;
            thresholdBypass = 0x00;
            break;

        default: // Default mode, balanced performance - same as configure(0)
            sigCountMax     = 0x80; // Default
            acqConfigReg    = 0x08; // Default
            refCountMax     = 0x05; // Default
            thresholdBypass = 0x00; // Default
            break;
    }

    i2cWrite(LLv3_SIG_CNT_VAL,   &sigCountMax    , 1, lidarliteAddress);
    i2cWrite(LLv3_ACQ_CONFIG,    &acqConfigReg   , 1, lidarliteAddress);
    i2cWrite(LLv3_REF_CNT_VAL,   &refCountMax    , 1, lidarliteAddress);
    i2cWrite(LLv3_THRESH_BYPASS, &thresholdBypass, 1, lidarliteAddress);
} /* LIDARLite_v3::configure */

/*------------------------------------------------------------------------------
  Set I2C Address
  Set Alternate I2C Device Address. See Operation Manual for additional info.
  
  Parameters
  ------------------------------------------------------------------------------
  newAddress: desired secondary I2C device address
  disableDefault: a non-zero value here means the default 0x62 I2C device
    address will be disabled.
  lidarliteAddress: Default 0x62. Fill in new address here if changed. See
    operating manual for instructions.
------------------------------------------------------------------------------*/
void LIDARLite_v3::setI2Caddr(__u8 newAddress, __u8 disableDefault, __u8 lidarliteAddress)
{
    __u8 dataBytes[2];

    // Read UNIT_ID serial number bytes and write them into I2C_ID byte locations
    i2cRead ((LLv3_UNIT_ID_HIGH | 0x80), dataBytes, 2, lidarliteAddress);
    i2cWrite(LLv3_I2C_ID_HIGH,           dataBytes, 2, lidarliteAddress);

    // Write the new I2C device address to registers
    dataBytes[0] = newAddress;
    i2cWrite(LLv3_I2C_SEC_ADR,           dataBytes, 1, lidarliteAddress);

    // Enable the new I2C device address using the default I2C device address
    dataBytes[0] = 0;
    i2cWrite(LLv3_I2C_CONFIG,            dataBytes, 1, lidarliteAddress);

    // If desired, disable default I2C device address (using the new I2C device address)
    if (disableDefault)
    {
        dataBytes[0] = (1 << 3); // set bit to disable default address
        i2cWrite(LLv3_I2C_CONFIG, dataBytes, 1, newAddress);
    }
} /* LIDARLite_v3::setI2Caddr */

/*------------------------------------------------------------------------------
  Take Range
  Initiate a distance measurement by writing to register 0x00.
  
  Parameters
  ------------------------------------------------------------------------------
  lidarliteAddress: Default 0x62. Fill in new address here if changed. See
    operating manual for instructions.
------------------------------------------------------------------------------*/
void LIDARLite_v3::takeRange(__u8 lidarliteAddress)
{
    __u8 commandByte = 0x04;

    i2cWrite(LLv3_ACQ_CMD, &commandByte, 1, lidarliteAddress);
} /* LIDARLite_v3::takeRange */

/*------------------------------------------------------------------------------
  Wait for Busy Flag
  Blocking function to wait until the Lidar Lite's internal busy flag goes low
  
  Parameters
  ------------------------------------------------------------------------------
  lidarliteAddress: Default 0x62. Fill in new address here if changed. See
    operating manual for instructions.
------------------------------------------------------------------------------*/
void LIDARLite_v3::waitForBusy(__u8 lidarliteAddress)
{
    __u8  busyFlag;

    do  // Loop until device is not busy
    {
        busyFlag = getBusyFlag(lidarliteAddress);
    } while (busyFlag);
} /* LIDARLite_v3::waitForBusy */

/*------------------------------------------------------------------------------
  Get Busy Flag
  Read BUSY flag from device registers. Function will return 0x00 if not busy.
  
  Parameters
  ------------------------------------------------------------------------------
  lidarliteAddress: Default 0x62. Fill in new address here if changed. See
    operating manual for instructions.
------------------------------------------------------------------------------*/
__u8 LIDARLite_v3::getBusyFlag(__u8 lidarliteAddress)
{
    __u8  statusByte = 0;
    __u8  busyFlag; // busyFlag monitors when the device is done with a measurement

    // Read status register to check busy flag
    i2cRead(LLv3_STATUS, &statusByte, 1, lidarliteAddress);

    // STATUS bit 0 is busyFlag
    busyFlag = statusByte & 0x01;

    return busyFlag;
} /* LIDARLite_v3::getBusyFlag */

/*------------------------------------------------------------------------------
  Read Distance
  Read and return result of distance measurement.

  Parameters
  ------------------------------------------------------------------------------
  lidarliteAddress: Default 0x62. Fill in new address here if changed. See
    operating manual for instructions.
------------------------------------------------------------------------------*/
__u16 LIDARLite_v3::readDistance(__u8 lidarliteAddress)
{
    __u8  distBytes[2] = {0};

    // Read two bytes from register 0x0f and 0x10 (autoincrement)
    i2cRead((LLv3_DISTANCE | 0x80), distBytes, 2, lidarliteAddress);

    // Shift high byte and OR in low byte
    return ((distBytes[0] << 8) | distBytes[1]);
} /* LIDARLite_v3::readDistance */

/*------------------------------------------------------------------------------
  Write
  Perform I2C write to device.

  Parameters
  ------------------------------------------------------------------------------
  regAddr:   register address to write to
  dataBytes: pointer to data bytes to write
  numBytes:  number of bytes to write
  lidarliteAddress: Default 0x62. Fill in new address here if changed. See
    operating manual for instructions.
------------------------------------------------------------------------------*/
__s32 LIDARLite_v3::i2cWrite(__u8 regAddr,  __u8 * dataBytes,
                             __u8 numBytes, __u8 lidarliteAddress)
{
    __u8 buffer[2];
    __u8 i;
    __s32 result;

    i2c_connect(lidarliteAddress);

    for (i=0 ; i<numBytes ; i++)
    {
        buffer[0] = regAddr + i;
        buffer[1] = dataBytes[i];
        result   |= write(file_i2c, buffer, 2);
    }

    return result;
} /* LIDARLite_v3::i2cWrite */

/*------------------------------------------------------------------------------
  Read
  Perform I2C read from device.

  Parameters
  ------------------------------------------------------------------------------
  regAddr:   register address to write to
  dataBytes: pointer to array to place read bytes
  numBytes:  number of bytes in 'dataBytes' array to read (32 bytes max)
  lidarliteAddress: Default 0x62. Fill in new address here if changed. See
  operating manual for instructions.
------------------------------------------------------------------------------*/
__s32 LIDARLite_v3::i2cRead(__u8 regAddr,  __u8 * dataBytes,
                            __u8 numBytes, __u8 lidarliteAddress)
{
    __u8 buffer;

    i2c_connect(lidarliteAddress);

    buffer = regAddr;

    write(file_i2c, &buffer, 1);
    return read(file_i2c, dataBytes, numBytes);
} /* LIDARLite_v3::i2cRead */

/*------------------------------------------------------------------------------
  Correlation Record Read
  The correlation record used to calculate distance can be read from the device.
  It has a bipolar wave shape, transitioning from a positive going portion to a
  roughly symmetrical negative going pulse. The point where the signal crosses
  zero represents the effective delay for the reference and return signals.
  
  Process
  ------------------------------------------------------------------------------
  1.  Take a distance reading (there is no correlation record without at least
      one distance reading being taken)
  2.  Set test mode select by writing 0x07 to register 0x40
  3.  For as many points as you want to read from the record (max is 1024) ...
      1.  Read two bytes from 0x52
      2.  The Low byte is the value from the record
      3.  The high byte is the sign from the record
  Parameters
  ------------------------------------------------------------------------------
  numberOfReadings: Default = 256. Maximum = 1024
  corrValues:       pointer to memory location to store the correlation record
                    ** Two bytes for every correlation value must be
                       allocated by calling function
  lidarliteAddress: Default 0x62. Fill in new address here if changed. See
    operating manual for instructions.
------------------------------------------------------------------------------*/
void LIDARLite_v3::correlationRecordRead(__s16 * correlationArray,
                                         __u16 numberOfReadings,
                                         __u8  lidarliteAddress)
{
    __u16  i = 0;
    __u8   dataBytes[2];
    __s16  correlationValue;
    __u8 * correlationValuePtr = (__u8 *) &correlationValue;

    //  Select memory bank
    dataBytes[0] = 0xc0;
    i2cWrite(LLv3_ACQ_SETTINGS, dataBytes, 1, lidarliteAddress);

    // Test mode enable
    dataBytes[0] = 0x07;
    i2cWrite(LLv3_COMMAND, dataBytes, 1, lidarliteAddress);

    for (i=0 ; i<numberOfReadings ; i++)
    {
        i2cRead((LLv3_CORR_DATA | 0x80), dataBytes, 2, lidarliteAddress);

        // First byte read is the magnitude of the data point
        correlationValuePtr[0] = dataBytes[0];

        // Second byte is the sign byte
        if (dataBytes[1])
            correlationValuePtr[1] = 0xff; // Artificially sign extend
        else
            correlationValuePtr[1] = 0x00;

        correlationArray[i] = correlationValue;
    }

    // Test mode disable
    dataBytes[0] = 0;
    i2cWrite(LLv3_COMMAND, dataBytes, 1, lidarliteAddress);
} /* LIDARLite_v3::correlationRecordRead */
