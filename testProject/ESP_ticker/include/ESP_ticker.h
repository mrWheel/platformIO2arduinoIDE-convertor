/* 
***************************************************************************  
**  Program  : ESP_ticker.h, part of ESP_ticker
**
**  Copyright (c) 2021 .. 2024 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
//#include <ESP8266WebServer.h>
#include <LittleFS.h>
#include <TelnetStream.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

#include "allDefines.h"
#include "networkStuff.h"
#include "parola_Fonts_data.h"
#include "FSexplorer.h"
#include "helperStuff.h"
#include "jsonStuff.h"
#include "littlefsStuff.h"
#include "newsapi_org.h"
#include "restAPI.h"
#include "sendIndexPage.h"
#include "settingStuff.h"
#include "WeerliveClass.h"
#include "TimeSync.h"


WiFiClient weerliveClient;
Weerlive weerlive(weerliveClient);

// WiFi Server object and parameters
ESP8266WebServer httpServer(80);
#ifdef USE_UPDATE_SERVER
  ESP8266HTTPUpdateServer httpUpdater(true);
#endif

void splitNewsNoWords(const char *noNo);

bool      Verbose = false;
char      cDate[15], cTime[10];
uint32_t  nrReboots;
// Global message buffers shared by Wifi and Scrolling functions
char      cMsg[NEWS_SIZE];
char      tempMessage[LCL_SIZE] = "";
char      lastWeerMessage[WEER_SIZE] = {};
uint8_t   msgType;
char      actMessage[WEER_SIZE], timeMsg[20];
char      onTickerMessage[LCL_SIZE] = {};
char      fileMessage[LCL_SIZE];
uint8_t   newsMsgID   = 0;
uint8_t   localMsgID  = 0;
int16_t   valueLDR, valueIntensity;
char      fChar[10];
String    lastReset   = "";
uint32_t  timeTimer   = 0;
uint32_t  ntpTimer    = millis() + 30000;
uint32_t  weerTimer   = 0;
uint32_t  newsapiTimer = 0;
uint32_t  revisionTimer = 0;
String    noWords[MAX_NO_NO_WORDS+1];
char      settingHostname[HOSTNAME_SIZE];
char      settingNewsNoWords[LCL_SIZE];
uint8_t   settingLocalMaxMsg, settingTextSpeed, settingMaxIntensity;
uint16_t  settingLDRlowOffset, settingLDRhighOffset;
char      settingWeerLiveAUTH[WEER_AUTH_SIZE], settingWeerLiveLocation[WEER_LIVE_LOC_SIZE];
uint8_t   settingWeerLiveInterval;
char      settingNewsAUTH[NEWS_AUTH_SIZE];
uint8_t   settingNewsInterval, settingNewsMaxMsg;
bool      LittleFSmounted; 
time_t    now; 
struct tm timeinfo;
bool      timeSynced = false;


TimeSync        timeSync;

const char *weekDayName[]  {  "Unknown", "Zondag", "Maandag", "Dinsdag", "Woensdag"
                            , "Donderdag", "Vrijdag", "Zaterdag", "Unknown" };
const char *flashMode[]    { "QIO", "QOUT", "DIO", "DOUT", "Unknown" };


/***************************************************************************
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the
* "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit
* persons to whom the Software is furnished to do so, subject to the
* following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
* OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
* THE USE OR OTHER DEALINGS IN THE SOFTWARE.
* 
****************************************************************************
*/