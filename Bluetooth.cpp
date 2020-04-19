/*
    Bluetooth.cpp
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

#include "Bluetooth.h"
int BlueTooth::powerOn(void)
{
		if (0 == bluetoothPower)
		{
				if (0 == sendCmdAndWaitForResp("AT+BTSTATUS?\r\n", "+BTSTATUS: 0", DEFAULT_TIMEOUT))
				{
						clearSerial();
						if (0 != sendCmdAndWaitForResp("AT+BTPOWER=1\r\n", "OK", DEFAULT_TIMEOUT))
						{
								ERROR("\r\nERROR:bluetoothPowerOn\r\n");
								clearSerial();
								return -1;
						}
						else
						{
								clearSerial();
								delay(2000);
								bluetoothPower = 1;
						}
				}
				else
				{
						bluetoothPower = 1;
				}
		}
		clearSerial();
		return 0;
}

int BlueTooth::powerOff(void)
{
		if (1 == bluetoothPower)
		{
				if (0 != sendCmdAndWaitForResp("AT+BTSTATUS?\r\n", "+BTSTATUS: 0", DEFAULT_TIMEOUT))
				{
						clearSerial();
						if (0 != sendCmdAndWaitForResp("AT+BTPOWER=0\r\n", "OK", DEFAULT_TIMEOUT))
						{
								ERROR("\r\nERROR:bluetoothPowerOff\r\n");
								clearSerial();
								return -1;
						}
						else
						{
								bluetoothPower = 0;
						}
				}

		}
		clearSerial();
		return 0;
}

int BlueTooth::forcePowerOff(void)
{
		sendCmdAndWaitForResp("AT+BTPOWER=0\r\n", "OK", DEFAULT_TIMEOUT);
		bluetoothPower = 0;
		return 0;
}

int BlueTooth::getHostDeviceName(char* deviceName)
{
		int bufferlen = 200;
		char blueBuffer[bufferlen];
		char *s,*p;
		int i = 0;
		sendCmd("AT+BTHOST?\r\n");
		readBufferRaw(blueBuffer,bufferlen,DEFAULT_TIMEOUT);
		if(NULL == (s = strstr(blueBuffer,"+BTHOST:")))
		{
				ERROR("\r\nERROR: get host device name error\r\n");
				return -1;
		}
		p = s + 9;            //+BTHOST: SIM808,48:e6:c0:18:62:60
		while(*(p) != ',')
		{
				deviceName[i++] = *p;
				p++;
		}
		deviceName[i] = '\0';
		return i;
}

int BlueTooth::scanForTargetDeviceName(char* deviceName)
{
		int bufferlen = 200;
		char blueBuffer[bufferlen];
		char *s;
		clearSerial();
		cleanBuffer(blueBuffer, bufferlen);
		sendCmd("AT+BTSCAN=1,20\r\n");             //scan 20s
		readBufferRaw(blueBuffer,bufferlen,20);            //+BTSCAN: 0,1,"E-test",34:43:0b:07:0f:58,-42
		DEBUG(blueBuffer);
		if(NULL == (s = strstr(blueBuffer,deviceName)))
		{
				ERROR("\r\nERROR: scan For Target Device error\r\n");
				return -1;
		}
		targetDeviceID = atoi(s-3);
		return targetDeviceID;
}

int BlueTooth::scanForTargetDeviceAddress(char* deviceName)
{
		int bufferlen = 512;
		char blueBuffer[bufferlen];
		char *s;
		clearSerial();
		cleanBuffer(blueBuffer, bufferlen);
		sendCmd("AT+BTSCAN=1,20\r\n");             //scan 20s
		readBufferRaw(blueBuffer,bufferlen,20);            //+BTPAIR: 1,"ME863",5c:6b:32:91:00:d1 --- +BTSCAN: 0,1,"E-test",34:43:0b:07:0f:58,-42
		if(NULL == (s = strstr(blueBuffer,deviceName)))
		{
				ERROR("\r\nERROR: scan For Target Device error\r\n");
				return -1;
		}
		targetDeviceID = atoi(s-11);
		return targetDeviceID;
}

int BlueTooth::getDeviceId(char* deviceName)
{
		int bufferlen = 200;
		char blueBuffer[bufferlen];
		char *s;
		cleanBuffer(blueBuffer, bufferlen);
		clearSerial();
		sendCmd("AT+BTSTATUS?\r\n");
		//DEBUG(blueBuffer);
		readBufferRaw(blueBuffer,bufferlen,10);            //less than 10 sec connection problem
		clearSerial();
		if(NULL == (s = strstr(blueBuffer,deviceName)))
		{
				ERROR("\r\nERROR: scan For Target Device error\r\n");
				return -1;
		}
		targetDeviceID = atoi(s-11);
		return targetDeviceID;
}

int BlueTooth::sendPairingReqstToDevice(int deviceID, int pin)
{
		int bufferlen = 30;
		char cmd[bufferlen];
		if(0 == deviceID)
		{
				return -1;
		}
		sprintf(cmd,"AT+BTPAIR=0,%d\r\n",deviceID);
		if(0 != sendCmdAndWaitForResp(cmd, "OK", 20))
		{
				clearSerial();
				ERROR("\r\nERROR: BTPAIRING\r\n");
				return -1;
		}
		cleanBuffer(cmd, bufferlen);
		clearSerial();
		sprintf(cmd,"AT+BTPAIR=2,%d\r\n",pin);
		if(0 != sendCmdAndWaitForResp(cmd, "OK", 5))
		{
				ERROR("\r\nERROR: AT+BTPAIR1\r\n");
				return -1;
		}

		/*if(0 != sendCmdAndWaitForResp("BTSTATUS?\r\n", "BTSTATUS: 0\r\n", 5))
		   {
		    ERROR("\r\nERROR: BTSTATUS?\r\n");
		    return -1;
		   }*/
		//clearSerial pairing issue
		//clearSerial();
		return 0;
}
int BlueTooth::unPair(int deviceID)
{
		char cmd[30];
		sprintf(cmd,"AT+BTUNPAIR=%d\r\n",deviceID);
		if(0 != sendCmdAndWaitForResp(cmd, "OK", DEFAULT_TIMEOUT))
		{
				ERROR("\r\nERROR: AT+BTUNPAIR\r\n");
				return -1;
		}
		return 0;
}

int BlueTooth::acceptPairing(void)
{
		sendCmd("AT+BTPAIR=1,1\r\n");
		clearSerial();
		return 0;
}

int BlueTooth::acceptConnect(void)
{
		if(0 != sendCmdAndWaitForResp("AT+BTACPT=1\r\n", "OK", DEFAULT_TIMEOUT))
		{
				ERROR("\r\nERROR:AT+BTACPT\r\n");
				return -1;
		}
		return 0;
}
int BlueTooth::disconnect(int deviceID)
{
		char cmd[30];
		if(0 == targetDeviceID)
				return -1;
		sprintf(cmd,"AT+BTDISCONN=%d\r\n",targetDeviceID);
		if(0 != sendCmdAndWaitForResp(cmd, "OK", DEFAULT_TIMEOUT))
		{
				ERROR("\r\nERROR: AT+BTDISCONN\r\n");
				return -1;
		}
		return 0;
}

int BlueTooth::loopHandle(void)
{
		int bufferlen = 100;
		char blueBuffer[bufferlen];
		cleanBuffer(blueBuffer,bufferlen);
		while(1)
		{
				if(serialBKSIM808.available())
				{
						break;
				}
				delay(1000);
		}
		readBufferRaw(blueBuffer,bufferlen,DEFAULT_TIMEOUT);

		if(NULL != strstr(blueBuffer,"+BTPAIRING:"))
		{
				if(0 != acceptPairing())
				{
						return -1;
						ERROR("\r\nERROR:bluetoothAcceptPairing\r\n");
				}
		}
		if((NULL != strstr(blueBuffer,"+BTCONNECTING:")) && (NULL != strstr(blueBuffer,"SPP")))
		{
				if(0 != acceptConnect())
				{
						return -1;
						ERROR("\r\nERROR:bluetoothAcceptConnecting\r\n");
				}
		}
		return 0;
}

int BlueTooth::connectInSPP(int deviceID)   //Serial Port Profile
{
		int bufferlen = 200;
		int cmdlen = 20;
		char blueBuffer[bufferlen];
		char cmd[cmdlen];
		char* s;

		delay(1000);
		clearSerial();
		//delay(1000);
		sprintf(cmd, "AT+BTGETPROF=%d\r\n", deviceID);
		sendCmd(cmd);
		//sendCmd("WAIT=4\r\n");
		readBufferRaw(blueBuffer, bufferlen, DEFAULT_TIMEOUT);
		if (NULL == (s = strstr(blueBuffer, "\"SPP\"")))
		{
				ERROR("\r\nERROR: No SPP Profile\r\n");
				return -1;
		}
		if (DEBUGMODE)
		{
				DEBUG("\r\nSPP:OK\r\n");
		}
		cleanBuffer(blueBuffer, bufferlen);
		cleanBuffer(cmd, cmdlen);
		clearSerial();
		sprintf(cmd, "AT+BTCONNECT=%d,%d\r\n", deviceID, 4);
		if (0 != sendCmdAndWaitForResp(cmd, "OK\r\n", DEFAULT_TIMEOUT))
		{
				ERROR("\r\nERROR:AT+BTCONNECT\r\n");
				return -1;
		}
		if (DEBUGMODE)
		{
				DEBUG("\r\nAT+BTCONNECT:OK\r\n");
		}
		sendCmdTimeout("WAIT=4\r\n",DEFAULT_TIMEOUT);
		delay(4000);
		clearSerial();
		return 0;
}

int BlueTooth::recvInSPP(char* data)
{
		unsigned long timerStart, timerEnd;
		int bufferlen = 130;
		char blueBuffer[bufferlen];
		char* p = NULL;
		int count = 0;
		timerStart = millis();
		//clearSerial();
		if (0 != sendCmdAndWaitForResp("AT+BTSPPGET=0\r\n", "OK\r\n", DEFAULT_TIMEOUT))
		{
				ERROR("\r\nERROR:AT+BTSPPGET\r\n");
				return -1;
		}
		if (DEBUGMODE)
		{
				DEBUG("\r\nAT+BTSPPGET:OK\r\n");
		}
		//sendCmdTimeout("WAIT=4\r\n", 4);
		//clearSerial();
		while (1)
		{
				if (serialBKSIM808.available())
				{
						break;
				}
				timerEnd = millis();
				if (timerEnd - timerStart > 1000 * 3)
				{
						break;
				}
		}

		readBuffer(blueBuffer, bufferlen);
		p = strstr(blueBuffer, "+BTSPPDATA:");
		if (NULL != p)
		{
				p += 11;
				while (count != 2)
				{
						if (*(p++) == ',')
						{
								count++;
						}
				}
				int i = 0;
				while (*(p++) != '$')
				{
						data[i++] = *p;
				}
		}
		else
		{
				ERROR("\r\nERROR: get data error\r\n");
				return -1;
		}
		return 0;
}

int BlueTooth::sendInSPP(char* data)
{
		//sendCmd("WAIT=2\r\n");
		//delay(4000);
		if (0 != sendCmdAndWaitForResp("AT+BTSPPSEND\r\n", ">", DEFAULT_TIMEOUT))
		{
				ERROR("\r\nERROR:AT+BTSPPSEND\r\n");
				return -1;
		}
		//clearSerial();
		sendCmd(data);
		sendEndMark();
		return waitForResp("SEND OK\r\n", DEFAULT_TIMEOUT);
}

int BlueTooth::dumpUsbkey()
{
		if (0 != sendCmdAndWaitForResp("AT+BTSPPSEND\r\n", ">", DEFAULT_TIMEOUT))
		{
				ERROR("\r\nERROR:AT+BTSPPSEND\r\n");
				return -1;
		}
		//clearSerial();
		sendDump();
		sendEndMark();
		return waitForResp("SEND OK\r\n", DEFAULT_TIMEOUT);
}
