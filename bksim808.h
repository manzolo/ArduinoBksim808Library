/*
    BKSIM808.h
    A library for SeeedStudio seeeduino GPRS shield

    Copyright (c) 2013 seeed technology inc.
    Author        :   lawliet zou
    Create Time   :   Dec 2013
    Change Log    :

    The MIT License (MIT)

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
 */
#ifndef __BKSIM808_H__
#define __BKSIM808_H__

#include "Arduino.h"
#include <SoftwareSerial.h>

#define TRUE                    1
#define FALSE                   0

#define BKSIM808_TX_PIN           10
#define BKSIM808_RX_PIN           11
#define BKSIM808_POWER_PIN        9
#define BKSIM808_POWER_STATUS     12

#define UART_DEBUG

#define DEBUGMODE false
#define WAITFORGSM true

#ifdef UART_DEBUG
#define ERROR(x)            if (DEBUGMODE) {Serial.print(x);}
#define DEBUG(x)            if (DEBUGMODE) {Serial.print(x);}
#else
#define ERROR(x)
#define DEBUG(x)
#endif

#define DEFAULT_TIMEOUT     5
/** BKSIM808 class.
    Used for BKSIM808 communication. attention that BKSIM808 module communicate with MCU in serial protocol
 */
class BKSIM808
{

private:
        String gpsstate, gpstimegps, gpslatitude, gpslongitude;
public:
        /** Create BKSIM808 Instance
            @param tx   uart transmit pin to communicate with BKSIM808
            @param rx   uart receive pin to communicate with BKSIM808
            @param baudRate baud rate of uart communication
         */
        BKSIM808(int baudRate) : serialBKSIM808(BKSIM808_TX_PIN, BKSIM808_RX_PIN)
        {
                //powerPin = BKSIM808_POWER_PIN;
                //pinMode(powerPin, OUTPUT);
                serialBKSIM808.begin(baudRate);
        };
        /** Power on BKSIM808
         */
        void preInit(void);

        /** Power on Gprs module
         */
        int gprsConnect(char* apn, unsigned int timeout);

        /** Power off Gprs module
         */
        int gprsDisconnect(unsigned int timeout);

        int sendWebserverPostData(char* serverurl, char* postParams,unsigned int timeout);

        /** Reset Gps information
         */
        void resetgpsinfo(void);

        /** send sms from BKSIM808 module
            @param  number  Number to send sms
            @param  sms   Text to send sms
            @param  timeout time to wait for reading from BKSIM808 module
            @returns
                0 on success
                -1 on error
         */

        int sendSmsMsg(char* number, char* sms, unsigned int timeout);

        /** get Gps module informations
            @param  timeout time to wait for reading from BKSIM808 module
            @returns
                0 on success
                -1 on error
         */
        int getGpsData(const int timeout);

        /** Get Gps Latitude
         */
        String getGpsLatitude();

        /** Get Gps Longitude
         */
        String getGpsLongitude();

        /** Get Gps UTC time
         */
        String getGpsTime();

        /** read from Gps module Gps informations
            @param  timeout time to wait for reading from BKSIM808 module
            @returns
                0 on success
                -1 on error
         */
        int getGpsInformation(int retry);

        /** Try to power on Gps module
         */
        int gpsPowerOn(unsigned int timeout);

        /** Try to power off Gps module
         */
        int gpsPowerOff(unsigned int timeout);

        /** Check if BKSIM808 readable
         */
        int checkReadable(void);

        /** read from BKSIM808 module and save to buffer array (it convert \r\n to $
            @param  buffer  buffer array to save what read from BKSIM808 module
            @param  count   the maximal bytes number read from BKSIM808 module
            @param  timeout time to wait for reading from BKSIM808 module
            @returns
                0 on success
                -1 on error
         */

        int readBuffer(char* buffer, int count, unsigned int timeout = DEFAULT_TIMEOUT);

        /** read from BKSIM808 module and save to buffer array
            @param  buffer  buffer array to save what read from BKSIM808 module
            @param  count   the maximal bytes number read from BKSIM808 module
            @param  timeout time to wait for reading from BKSIM808 module
            @returns
                0 on success
                -1 on error
         */

        int readBufferRaw(char* buffer, int count, unsigned int timeout = DEFAULT_TIMEOUT);


        /** clean Buffer
            @param buffer   buffer to clean
            @param count    number of bytes to clean
         */
        void cleanBuffer(char* buffer, int count);

        /** send AT command to BKSIM808 module
            @param cmd  command array which will be send to GPRS module
         */
        void sendCmd(const char* cmd);

        /** send AT command and wait for response
            @param cmd  command array which will be send to GPRS module
         */
        void sendCmdTimeout(const char* cmd, unsigned int timeout = DEFAULT_TIMEOUT);


        /** send "AT" to BKSIM808 module
         */
        int sendATTest(void);

        /** send '0xF0' to BKSIM808 Module
         */
        void sendDump(void);

        /** send '0x1A' to BKSIM808 Module
         */
        void sendEndMark(void);

        /** check BKSIM808 module response before time out
            @param  *resp   correct response which BKSIM808 module will return
            @param  *timeout    waiting seconds till timeout
            @returns
                0 on success
                -1 on error
         */
        int waitForResp(const char* resp, unsigned timeout);

        /** send AT command to GPRS module and wait for correct response
            @param  *cmd    AT command which will be send to GPRS module
            @param  *resp   correct response which GPRS module will return
            @param  *timeout    waiting seconds till timeout
            @returns
                0 on success
                -1 on error
         */
        int sendCmdAndWaitForResp(const char* cmd, const char* resp, unsigned timeout);


        /** used for serial debug, you can specify tx and rx pin and then communicate with GPRS module with common AT commands
         */
        void serialDebug(void);

        /** used for clear serial buffer
         */
        int clearSerial(void);

        /** Gps On flag
         */
        bool gpsIsOn;

        /** Board power pin
         */
        int powerPin;
        SoftwareSerial serialBKSIM808;

private:

};

#endif
