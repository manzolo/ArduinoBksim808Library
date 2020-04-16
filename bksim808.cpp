/*
    sim800.cpp
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

#include "bksim808.h"

void BKSIM808::preInit(void)
{
    while (sendATTest() != 0);
    clearSerial();
    if (DEBUGMODE)
    {
        sendCmd("AT+CMEE=2\r\n");
        clearSerial();
    }
    while (sendCmdAndWaitForResp("AT+CGMR\r\n", "Revision:1418B02SIM808M32_BT", DEFAULT_TIMEOUT) != 0);
    sendCmd("AT+BTPOWER?\r\n");
    clearSerial();
    sendCmd("AT+BTPOWER=0\r\n");
    clearSerial();
    sendCmd("AT+BTPOWER=1\r\n");
    clearSerial();
    sendCmd("AT+CGNSPWR=0\r\n");
    clearSerial();
    sendCmd("AT+SAPBR=0,1\r\n");
    resetgpsinfo();
    clearSerial();
    //while (sendCmdAndWaitForResp("AT+BTSTATUS?\r\n", "BTSTATUS: 5", DEFAULT_TIMEOUT) != 0);
    if (DEBUGMODE)
    {
        DEBUG("BkSim808 ready...\r\n");
    }
}

int BKSIM808::checkReadable(void)
{
    return serialBKSIM808.available();
}

int BKSIM808::sendSmsMsg(char* number, char* sms, unsigned int timeout)
{
    //put the modem into text mode
    char cmd[100];
    if (0 != sendCmdAndWaitForResp("AT+CMGF=1\r\n", "OK", timeout))
    {
        ERROR("\r\nERROR:AT+CMGF\r\n");
        return -1;
    }
    sprintf(cmd, "AT+CMGS=\"%s\"\r\n", number);
    if (0 != sendCmdAndWaitForResp(cmd, ">", timeout))
    {
        ERROR("\r\nERROR:AT+CMGS\r\n");
        return -1;
    }
    sendCmd(sms);
    sendEndMark();
    return 0;
}

String BKSIM808::getGpsLatitude()
{
    return this->gpslatitude;
}

String BKSIM808::getGpsLongitude()
{
    return this->gpslongitude;
}

String BKSIM808::getGpsTime()
{
    return this->gpstimegps;
}

void BKSIM808::resetgpsinfo()
{
    this->gpsstate = "";
    this->gpstimegps = "";
    this->gpslatitude = "";
    this->gpslongitude = "";
}
int BKSIM808::gpsPowerOn(unsigned int timeout)
{
    clearSerial();
    if( 0 != sendCmdAndWaitForResp("AT+CGNSPWR=1\r\n", "OK\r\n", timeout) )
    {
        if( 0 != sendCmdAndWaitForResp("AT+CGNSPWR?\r\n", "OK\r\n", timeout) )
        {
            ERROR("\r\nERROR:GpsPowerOn\r\n");
            return -1;
        }
    }
    delay(10000);
    //sendCmd("WAIT=6\r\n");
    return 0;
}

int BKSIM808::gpsPowerOff(unsigned int timeout)
{
    clearSerial();
    if( 0 != sendCmdAndWaitForResp("AT+CGNSPWR=0\r\n", "OK\r\n", timeout) )
    {
        if( 0 != sendCmdAndWaitForResp("AT+CGNSPWR?\r\n", "OK\r\n", timeout) )
        {
            ERROR("\r\nERROR:GpsPowerOff\r\n");
            return -1;
        }
    }
    delay(1000);
    return 0;
}

int BKSIM808::getGpsInformation(int retry)
{
    this->resetgpsinfo();
    int loop = 0;
    int ret = -1;
    gpsPowerOn(5);

    while (loop < retry)
    {
        loop++;
        delay(500);
        ;
        if ( this->getGpsData(10) == 0)
        {
            /*DEBUG("Found gps info\r\n");
            DEBUG("State  :" + this->gpsstate+"\r\n");
            DEBUG("Time  :" + this->gpstimegps+"\r\n");
            DEBUG("Latitude  :" + this->gpslatitude+"\r\n");
            DEBUG("Longitude  :" + this->gpslongitude+"\r\n");*/
            gpsPowerOff(5);
            return 0;
        }
        else
        {
            DEBUG("GPS not ready...\r\n");
        }
    }

    gpsPowerOff(5);

    return -1;
}
int BKSIM808::getGpsData(const int timeout)
{
    String data[5];
    long int time = millis();
    long int maxloop = 32767;
    long int idxmaxloop =0;
    int i = 0;

    delay(500);
    clearSerial();
    sendCmd("AT+CGNSINF\r\n");

    while ((time + (timeout * 1000)) > millis())
    {
        idxmaxloop++;
        while (serialBKSIM808.available())
        {
            char c = serialBKSIM808.read();
            if (c != ',')
            {
                data[i] += c;
                delay(50);
            }
            else
            {
                i++;
            }
            if (i == 5)
            {
                //delay(50);
                if (data[1]!="1")
                {
                    break;
                }
                this->gpsstate = data[1];
                this->gpstimegps = data[2];
                this->gpslatitude = data[3];
                this->gpslongitude = data[4];
                return 0;
            }
        }
        //DEBUG(i);
        if (idxmaxloop >= maxloop)
        {
            return -1;
        }
    }
    return -1;

}

int BKSIM808::readBuffer(char* buffer, int count, unsigned int timeout)
{
    int i = 0;
    unsigned long timerStart, timerEnd;
    timerStart = millis();
    while (1)
    {
        while (serialBKSIM808.available())
        {
            char c = serialBKSIM808.read();
            if (c == '\r' || c == '\n')
            {
                c = '$';
            }
            buffer[i++] = c;
            if (i > count - 1)
            {
                break;
            }
        }
        if (i > count - 1)
        {
            break;
        }
        timerEnd = millis();
        if (timerEnd - timerStart > 1000 * timeout)
        {
            break;
        }
    }
    while (serialBKSIM808.available())
    {
        serialBKSIM808.read();
    }
    if (DEBUGMODE)
    {
        DEBUG(buffer);
        DEBUG("\r\n");
    }
    return 0;
}

void BKSIM808::cleanBuffer(char* buffer, int count)
{
    for (int i = 0; i < count; i++)
    {
        buffer[i] = '\0';
    }
}

void BKSIM808::sendCmd(const char* cmd)
{
    if (DEBUGMODE)
    {
        DEBUG("sendCmd:\r\n");
        DEBUG(cmd);
        DEBUG("\r\n");
    }
    serialBKSIM808.write(cmd);
}

void BKSIM808::sendCmdTimeout(const char* cmd, unsigned int timeout)
{
    unsigned long timerStart, timerEnd;
    String buffer;
    timerStart = millis();

    serialBKSIM808.write(cmd);

    while (1)
    {
        if (serialBKSIM808.available())
        {
            char c = serialBKSIM808.read();

            if (DEBUGMODE)
            {
                buffer+=c;
            }
        }
        timerEnd = millis();
        if (timerEnd - timerStart > 1000 * timeout)
        {
            break;
        }
    }
    if (DEBUGMODE)
    {
        DEBUG(buffer);
        DEBUG("\r\n");
    }
}

int BKSIM808::sendATTest(void)
{
    int ret = sendCmdAndWaitForResp("AT\r\n", "OK\r\n", DEFAULT_TIMEOUT);
    return ret;
}

int BKSIM808::waitForResp(const char* resp, unsigned int timeout)
{
    int len = strlen(resp);
    int sum = 0;
    String buffer;
    unsigned long timerStart, timerEnd;
    timerStart = millis();

    while (1)
    {
        if (serialBKSIM808.available())
        {
            char c = serialBKSIM808.read();
            if (DEBUGMODE)
            {
                buffer+=c;
            }
            sum = (c == resp[sum]) ? sum + 1 : 0;
            if (sum == len)
            {
                break;
            }
        }
        timerEnd = millis();
        if (timerEnd - timerStart > 1000 * timeout)
        {
            return -1;
        }
    }

    while (serialBKSIM808.available())
    {
        serialBKSIM808.read();
    }
    if (DEBUGMODE)
    {
        DEBUG(buffer);
        DEBUG("\r\n");
    }
    return 0;
}

void BKSIM808::sendEndMark(void)
{
    serialBKSIM808.println((char)26);
}

void BKSIM808::sendDump(void)
{
    serialBKSIM808.println((char)240);
}

int BKSIM808::sendCmdAndWaitForResp(const char* cmd, const char* resp, unsigned timeout)
{
    sendCmd(cmd);
    if (DEBUGMODE)
    {
        DEBUG(cmd);
        DEBUG("\r\n");
    }
    return waitForResp(resp, timeout);
}

void BKSIM808::serialDebug(void)
{
    while (1)
    {
        if (serialBKSIM808.available())
        {
            Serial.write(serialBKSIM808.read());
        }
        if (Serial.available())
        {
            serialBKSIM808.write(Serial.read());
        }
    }
}

int BKSIM808::clearSerial(void)
{
    String buffer;
    int timeout = 1;
    int ret = -1;
    unsigned long timerStart, timerEnd;
    timerStart = millis();

    if (DEBUGMODE)
    {
        DEBUG("****START CLEAR SERIAL BUFFER****\r\n");
    }
    while (1)
    {
        if (serialBKSIM808.available())
        {
            char c = serialBKSIM808.read();
            if (DEBUGMODE)
            {
                buffer+=c;
            }
        }
        timerEnd = millis();
        if (timerEnd - timerStart > 1000 * timeout)
        {
            break;
        }

    }

    if (DEBUGMODE)
    {
        DEBUG(buffer);
        DEBUG("****END CLEAR SERIAL BUFFER****\r\n");
    }
    return 0;


}
int BKSIM808::gprsConnect(char* apn, unsigned int timeout)
{
    char cmd[1024];
    //sendCmd("AT+SAPBR=0,1\r\n");
    if( 0 == sendCmdAndWaitForResp("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n", "OK\r\n", timeout) )
    {
        sprintf(cmd, "AT+SAPBR=3,1,\"APN\",\"%s\"\r\n",apn);
        if( 0 == sendCmdAndWaitForResp(cmd, "OK\r\n", timeout) )
        {
            if( 0 == sendCmdAndWaitForResp("AT+SAPBR=3,1,\"USER\",\"\"\r\n", "OK\r\n", timeout) )
            {
                if( 0 == sendCmdAndWaitForResp("AT+SAPBR=3,1,\"PWD\",\"\"\r\n", "OK\r\n", timeout) )
                {
                    if( 0 == sendCmdAndWaitForResp("AT+SAPBR=1,1\r\n", "OK\r\n", timeout) )
                    {
                        if( 0 == sendCmdAndWaitForResp("AT+SAPBR=2,1\r\n", "OK\r\n", timeout) )
                        {
                            if( 0 == sendCmdAndWaitForResp("AT+CREG=1\r\n", "OK\r\n", timeout) )
                            {
                                if( 0 == sendCmdAndWaitForResp("AT+CIPSHUT\r\n", "OK\r\n", timeout) )
                                {
                                    if( 0 == sendCmdAndWaitForResp("AT+CGATT=1\r\n", "OK\r\n", timeout) )
                                    {
                                        delay(5000);
                                        return 0;
                                    }
                                    else
                                    {
                                        ERROR("\r\nERROR:AT+CGATT=1\r\n");
                                    }
                                }
                                else
                                {
                                    ERROR("\r\nERROR:AT+CIPSHUT\r\n");
                                }
                            }
                            else
                            {
                                ERROR("\r\nERROR:AT+CREG=1\r\n");
                            }
                        }
                        else
                        {
                            ERROR("\r\nERROR:SAPBR=2,1\r\n");
                        }
                    }
                    else
                    {
                        ERROR("\r\nERROR:SAPBR=1,1\r\n");
                    }
                }
                else
                {
                    ERROR("\r\nERROR:SAPBR PWD\r\n");
                }
            }
            else
            {
                ERROR("\r\nERROR:SAPBR USER\r\n");
            }
        }
        else
        {
            ERROR("\r\nERROR:SAPBR APN\r\n");
        }
    }
    else
    {
        ERROR("\r\nERROR:SAPBR CONTYPE\r\n");
    }
    return -1;

}

int BKSIM808::gprsDisconnect(unsigned int timeout)
{
    if( 0 == sendCmdAndWaitForResp("AT+SAPBR=0,1\r\n", "OK\r\n", timeout) )
    {
        sendCmd("AT+HTTPTERM\r\n");
        return 0;
    }
    sendCmd("AT+HTTPTERM\r\n");
    return -1;

}

int BKSIM808::sendWebserverJson(char* serverurl, char* device,char* volt,char* data,char* temp,char* batteryperc,char* latitude,char* longitude,unsigned int timeout)
{

    char cmd[1024];
    char postParams[1024];
    sendCmd("WAIT=6\r\n");
    if( 0 == sendCmdAndWaitForResp("AT+HTTPINIT\r\n", "OK\r\n", timeout) )
    {
        if( 0 == sendCmdAndWaitForResp("AT+HTTPPARA=\"CID\",1\r\n","OK\r\n", timeout) )
        {
            sprintf(cmd, "AT+HTTPPARA=\"URL\",\"%s\"\r\n",serverurl);

            if( 0 == sendCmdAndWaitForResp(cmd, "OK\r\n", timeout) )
            {
                if( 0 == sendCmdAndWaitForResp("AT+HTTPPARA=\"CONTENT\",\"application/json\"\r\n", "OK\r\n", timeout) )
                {
                    sprintf(postParams, "{\"device\" : \"%s\",  \"volt\" : \"%s\" ,\"data\" : \"%s\",  \"temp\" : \"%s\",   \"batteryperc\" : \"%s\",  \"latitude\" : \"%s\", \"longitude\" : \"%s\" }",device,volt,data,temp,batteryperc,latitude,longitude);
                    sprintf(cmd, "AT+HTTPDATA=%d,100000\r\n",strlen(postParams));
                    if( 0 == sendCmdAndWaitForResp(cmd, "DOWNLOAD\r\n", timeout) )
                    {
                        if( 0 == sendCmdAndWaitForResp(postParams, "OK\r\n", timeout) )
                        {
                            if( 0 == sendCmdAndWaitForResp("AT+HTTPACTION=1\r\n", "OK\r\n", timeout) )
                            {
                                //sendCmd("WAIT=4\r\n");
                                if( 0 == sendCmdAndWaitForResp("AT+HTTPREAD\r\n", "OK\r\n", timeout) )
                                {
                                    //sendCmd("WAIT=10\r\n");
                                    delay(10000);
                                    if( 0 == sendCmdAndWaitForResp("AT+HTTPSTATUS?\r\n", "OK\r\n", timeout) )
                                    {
                                        delay(5000);
                                        if( 0 == sendCmdAndWaitForResp("AT+HTTPTERM\r\n", "OK\r\n", timeout) )
                                        {

                                            clearSerial();
                                            waitForResp(",200\r\n", timeout);
                                            //clearSerial();
                                            return 0;
                                        }
                                        else
                                        {
                                            ERROR("\r\nERROR:AT+HTTPTERM\r\n");
                                        }
                                    }
                                    else
                                    {
                                        ERROR("\r\nERROR:AT+HTTPSTATUS\r\n");
                                    }
                                }
                                else
                                {
                                    ERROR("\r\nERROR:AT+HTTPREAD\r\n");
                                }
                            }
                            else
                            {
                                ERROR("\r\nERROR:AT+HTTPACTION=1\r\n");
                            }
                        }
                        else
                        {
                            ERROR("\r\nERROR:POST PARAMETERS\r\n");
                        }
                    }
                    else
                    {
                        ERROR("\r\nERROR:AT+HTTPDATA\r\n");
                    }
                }
                else
                {
                    ERROR("\r\nERROR:AT+HTTPPARA CONTENT\r\n");
                }
            }
            else
            {
                ERROR("\r\nERROR:AT+HTTPPARA URL\r\n");
            }
        }
        else
        {
            ERROR("\r\nERROR:AT+HTTPPARA CID\r\n");
        }
    }
    else
    {
        ERROR("\r\nERROR:AT+HTTPINIT\r\n");
    }
    return -1;
}
