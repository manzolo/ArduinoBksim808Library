/*
   BKSIM808.cpp
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
		//sendCmd("AT+CSCLK=0\r\n");
		while (sendATTest() != 0);
		while (sendATTest() != 0);
		if (WAITFORGSM) {
				do {
						//clearSerial();
						delay(1000);
				} while (sendCmdAndWaitForResp("AT&W\r\n", "OK\r\n", DEFAULT_TIMEOUT) != 0);
				do {
						//clearSerial();
						delay(1000);
				} while (sendCmdAndWaitForResp("AT+CFUN=0\r\n", "OK\r\n", DEFAULT_TIMEOUT) != 0);
				do {
						//clearSerial();
						delay(1000);
				} while (sendCmdAndWaitForResp("AT+CFUN=1,1\r\n", "OK\r\n", DEFAULT_TIMEOUT) != 0);
				do {
						//clearSerial();
						delay(1000);
				} while (sendCmdAndWaitForResp("ATE0\r\n", "OK\r\n", DEFAULT_TIMEOUT) != 0);
				do {
						//clearSerial();
						delay(1000);
				} while (sendCmdAndWaitForResp("AT+GMM\r\n", "OK\r\n", DEFAULT_TIMEOUT) != 0);
				do {
						//clearSerial();
						delay(1000);
				} while (sendCmdAndWaitForResp("AT+CLTS=1\r\n", "OK\r\n", DEFAULT_TIMEOUT) != 0);
				do {
						//clearSerial();
						delay(1000);
				} while (sendCmdAndWaitForResp("AT+CBATCHK=1\r\n", "OK\r\n", DEFAULT_TIMEOUT) != 0);


				do {
						//clearSerial();
						delay(1000);
				} while (sendCmdAndWaitForResp("AT+CPIN?\r\n", "CPIN: READY\r\n", DEFAULT_TIMEOUT) != 0);

				do {
						//sendCmd("WAIT=1\r\n");
						//clearSerial();
						delay(1000);
				} while (sendCmdAndWaitForResp("AT+CLCK=\"SC\",2\r\n", "CLCK: 0\r\n", DEFAULT_TIMEOUT) != 0);

				do {
						//clearSerial();
						delay(3000);
				} while (sendCmdAndWaitForResp("AT+CREG?\r\n", "CREG: 1,1\r\n", DEFAULT_TIMEOUT) != 0);

				do {
						//clearSerial();
						delay(3000);
				} while (sendCmdAndWaitForResp("AT+CSCS=\"GSM\"\r\n", "OK\r\n", DEFAULT_TIMEOUT) != 0);
				/*do {
				                //clearSerial();
				                delay(3000);
				   } while (sendCmdAndWaitForResp("AT+CGREG?\r\n", "CGREG: 0,1\r\n", 10) != 0);*/

		}
		do {
				//clearSerial();
				delay(5000);
		} while (sendCmdAndWaitForResp("AT+BTPOWER?\r\n", "OK\r\n", DEFAULT_TIMEOUT) != 0);
		//sendCmdTimeout("WAIT=6\r\n", 6);
		////clearSerial();
		while (sendCmdAndWaitForResp("AT\r\n", "OK\r\n", DEFAULT_TIMEOUT) != 0);
		//clearSerial();
		if (DEBUGMODE)
		{
				while (sendCmdAndWaitForResp("AT+CMEE=2\r\n", "OK\r\n", DEFAULT_TIMEOUT) != 0);
		}

		while (sendCmdAndWaitForResp("AT+CGMR\r\n", "Revision:1418B02SIM808M32_BT\r\n", DEFAULT_TIMEOUT) != 0);
		////clearSerial();
		//sendCmd("AT+CSCLK=1\r\n");
		gpsIsOn = false;
		if (DEBUGMODE)
		{
				DEBUG("BkSim808 ready...\r\n");
		}
		//digitalWrite(powerPin, HIGH);
}

int BKSIM808::checkReadable(void)
{
		return serialBKSIM808.available();
}

int BKSIM808::sendSmsMsg(char* number, char* sms, unsigned int timeout)
{
		//put the modem into text mode
		char cmd[100];
		if (0 != sendCmdAndWaitForResp("AT+CMGF=1\r\n", "OK\r\n", timeout))
		{
				ERROR("\r\nERROR:AT+CMGF\r\n");
				return -1;
		}
		delay(50);
		sprintf(cmd, "AT+CMGS=\"%s\"\r\n", number);
		if (0 != sendCmdAndWaitForResp(cmd, ">", timeout))
		{
				ERROR("\r\nERROR:AT+CMGS\r\n");
				return -1;
		}
		delay(1000);
		//serialBKSIM808.println("* * * Arduino GSM start * * *");
		sendCmd(sms);
		delay(500);
		sendEndMark();
		return waitForResp("OK\r\n",timeout);
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
		//clearSerial();
		resetgpsinfo();
		if( 0 != sendCmdAndWaitForResp("AT+CGNSPWR=1\r\n", "OK\r\n", timeout) )
		{
				//if( 0 != sendCmdAndWaitForResp("AT+CGNSPWR?\r\n", "OK\r\n", timeout) )
				//{
				ERROR("\r\nERROR:GpsPowerOn\r\n");
				return -1;
				//}
		}
		if (gpsIsOn)
		{
				delay(5000);
		}
		else
		{
				delay(45000);

		}
		//sendCmdAndWaitForResp("AT+CGNSSEQ?\r\n", "OK\r\n", timeout);  // Define the last NMEA sentence that parsed
		//sendCmdAndWaitForResp("AT+CGNSSEQ=\"RMC\"\r\n", "OK\r\n", timeout);  // Define the last NMEA sentence that parsed
		//sendCmdAndWaitForResp("AT+CGNSSEQ?\r\n", "OK\r\n", timeout); // Define the last NMEA sentence that parsed
		//clearSerial();
		//sendCmd("WAIT=6\r\n");
		return 0;
}

int BKSIM808::gpsPowerOff(unsigned int timeout)
{
		//clearSerial();
		if( 0 != sendCmdAndWaitForResp("AT+CGNSPWR=0\r\n", "OK\r\n", timeout) )
		{
				//if( 0 != sendCmdAndWaitForResp("AT+CGNSPWR?\r\n", "OK\r\n", timeout) )
				//{
				ERROR("\r\nERROR:GpsPowerOff\r\n");
				return -1;
				//}
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
		char *Fixstatus;
		char *UTCdatetime;
		char *latitude;
		char *logitude;
		char gpsBuffer[200];

		cleanBuffer(gpsBuffer,200);
		sendCmd("AT+CGNSINF\r\n");
		readBufferRaw(gpsBuffer,200,DEFAULT_TIMEOUT);

		// first field is GPS run status
		char *field = strtok( gpsBuffer, "," );
		Fixstatus = strtok( nullptr, "," );
		UTCdatetime = strtok( nullptr, "," );
		latitude = strtok( nullptr, "," );
		logitude = strtok( nullptr, "," );

		if (String(Fixstatus)!="1" || latitude == nullptr || logitude == nullptr)
		{
				gpsIsOn = false;
				return -1;
		}

		this->gpslatitude = String(latitude);
		this->gpslongitude = String(logitude);
		this->gpstimegps = String(UTCdatetime);
		this->gpsstate = String(Fixstatus);
		gpsIsOn = true;
		return 0;
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
				DEBUG("String readBuffer:");
				DEBUG("\r\n");
				DEBUG(buffer);
				DEBUG("\r\n");
				/*DEBUG("HEX readBuffer:");
				   DEBUG("\r\n");
				   for (int b = 0; b < count; b++)
				   {
				   Serial.println(buffer[b], HEX);
				   }*/
		}
		return 0;
}

int BKSIM808::readBufferRaw(char* buffer, int count, unsigned int timeout)
{
		int i = 0;
		unsigned long timerStart, timerEnd;
		timerStart = millis();
		while (1)
		{
				while (serialBKSIM808.available())
				{
						char c = serialBKSIM808.read();
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
		DEBUG("String readBufferRaw:");
		while (serialBKSIM808.available())
		{
				DEBUG(serialBKSIM808.read());
		}
		DEBUG("\r\n");
		DEBUG(buffer);
		DEBUG("\r\n");
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
						DEBUG(buffer);
						DEBUG("\r\n");
						return -1;
				}
		}

		while (serialBKSIM808.available())
		{
				DEBUG(serialBKSIM808.read());
		}
		DEBUG(buffer);
		DEBUG("\r\n");
		return 0;
}

int BKSIM808::isSerialReady(unsigned timeout){
		int ret = 0;
		unsigned long timerStart, timerEnd;
		timerStart = millis();

		while (1)
		{
				if (checkReadable())
				{
						ret = 1;
						break;
				}
				timerEnd = millis();
				if (timerEnd - timerStart > 1000 * timeout)
				{
						break;
				}

		}
		return ret;
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

int BKSIM808::cleanSerialBuffer(void)
{
		String buffer;
		int timeout = 1;
		int ret = -1;
		unsigned long timerStart, timerEnd;
		timerStart = millis();

		DEBUG("****START CLEAR SERIAL BUFFER****\r\n");
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
				if (timerEnd - timerStart > 1000 * 1)
				{
						break;
				}

		}

		DEBUG(buffer);
		DEBUG("****END CLEAR SERIAL BUFFER****\r\n");
		return 0;


}
int BKSIM808::gprsConnect(char* apn, unsigned int timeout)
{
		char cmd[40];
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
				//clearSerial();
				return 0;
		}
		sendCmd("AT+HTTPTERM\r\n");
		//clearSerial();
		return -1;

}

int BKSIM808::sendWebserverPostData(char* serverurl, char* postParams,unsigned int timeout)
{

		char cmd[1024];

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
										sprintf(cmd, "AT+HTTPDATA=%d,100000\r\n",strlen(postParams));
										if( 0 == sendCmdAndWaitForResp(cmd, "DOWNLOAD\r\n", timeout) )
										{
												//clearSerial();
												if( 0 == sendCmdAndWaitForResp(postParams, "OK\r\n", timeout) )
												{
														//clearSerial();
														if( 0 == sendCmdAndWaitForResp("AT+HTTPACTION=1\r\n", "OK\r\n", timeout) )
														{
																waitForResp("HTTPACTION: 1,200,",15);
																//clearSerial();
																if( 0 == sendCmdAndWaitForResp("AT+HTTPREAD\r\n", "OK\r\n", timeout) )
																{
																		waitForResp("HTTPREAD:", 15);
																		//clearSerial();
																		if( 0 == sendCmdAndWaitForResp("AT+HTTPSTATUS?\r\n", "OK\r\n", timeout) )
																		{
																				waitForResp("HTTPSTATUS: POST,0,0,0", 15);
																				//clearSerial();
																				if( 0 == sendCmdAndWaitForResp("AT+HTTPTERM\r\n", "OK\r\n", timeout) )
																				{
																						//clearSerial();
																						//delay(10000);
																						////clearSerial();
																						//waitForResp(",200\r\n", timeout);
																						////clearSerial();
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
