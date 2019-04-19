/*------------------------------------------------------------------------------
  LIDARLite_v3 Raspberry Pi Library

  This library provides quick access to basic functions of the LIDAR-Lite
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

#ifndef LIDARLite_v3_h
#define LIDARLite_v3_h

// LIDAR-Lite default I2C device address
#define LIDARLITE_ADDR_DEFAULT 0x62

// LIDAR-Lite internal register addresses
#define LLv3_ACQ_CMD       0x00
#define LLv3_STATUS        0x01
#define LLv3_SIG_CNT_VAL   0x02
#define LLv3_ACQ_CONFIG    0x04
#define LLv3_DISTANCE      0x0f
#define LLv3_REF_CNT_VAL   0x12
#define LLv3_UNIT_ID_HIGH  0x16
#define LLv3_UNIT_ID_LOW   0x17
#define LLv3_I2C_ID_HIGH   0x18
#define LLv3_I2C_ID_LOW    0x19
#define LLv3_I2C_SEC_ADR   0x1a
#define LLv3_THRESH_BYPASS 0x1c
#define LLv3_I2C_CONFIG    0x1e
#define LLv3_COMMAND       0x40
#define LLv3_CORR_DATA     0x52
#define LLv3_ACQ_SETTINGS  0x5d

class LIDARLite_v3
{
        __u32     file_i2c;
    public:
        __s32     i2c_init    (void);
        __s32     i2c_connect (__u8 lidarliteAddress = LIDARLITE_ADDR_DEFAULT);
        void      configure   (__u8 configuration = 0, __u8 lidarliteAddress = LIDARLITE_ADDR_DEFAULT);
        void      setI2Caddr  (__u8 newAddress, __u8 disableDefault, __u8 lidarliteAddress = LIDARLITE_ADDR_DEFAULT);
        __u16     readDistance(__u8 lidarliteAddress = LIDARLITE_ADDR_DEFAULT);
        void      waitForBusy (__u8 lidarliteAddress = LIDARLITE_ADDR_DEFAULT);
        __u8      getBusyFlag (__u8 lidarliteAddress = LIDARLITE_ADDR_DEFAULT);
        void      takeRange   (__u8 lidarliteAddress = LIDARLITE_ADDR_DEFAULT);
        __s32     i2cWrite    (__u8 regAddr, __u8 * dataBytes, __u8 numBytes, __u8 lidarliteAddress = LIDARLITE_ADDR_DEFAULT);
        __s32     i2cRead     (__u8 regAddr, __u8 * dataBytes, __u8 numBytes, __u8 lidarliteAddress = LIDARLITE_ADDR_DEFAULT);
        void      correlationRecordRead (__s16 * corrValues, __u16 numberOfReadings = 256, __u8 lidarliteAddress = LIDARLITE_ADDR_DEFAULT);
};

#endif
