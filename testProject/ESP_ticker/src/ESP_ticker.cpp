/*
***************************************************************************
**  Program  : ESP_ticker (lichtkrant)
**
**  See "allDefines.h" for _FW_VERSION
**
**  Copyright (c) 2021 .. 2024 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
*/


// Use the Parola library to scroll text on the display
// IP address for the ESP8266 is displayed on the scrolling display
// after startup initialisation and connected to the WiFi network.
//
// Connections for ESP8266 hardware SPI are:
// Vcc       3v3     LED matrices seem to work at 3.3V
// GND       GND     GND
// DIN        D7     HSPID or HMOSI
// CS or LD   D8     HSPICS or HCS
// CLK        D5     CLK or HCLK
//
// MD_MAX72XX library can be found at https://github.com/MajicDesigns/MD_MAX72XX
//

#include "ESP_ticker.h"

// HARDWARE SPI
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// Scrolling parameters
uint8_t  inFX, outFX;
textEffect_t  effect[] =
{
  PA_PRINT,
  //PA_SCAN_HORIZ,
  PA_SCROLL_LEFT,
  PA_WIPE,
  PA_SCROLL_UP_LEFT,
  PA_SCROLL_UP,
  PA_OPENING_CURSOR,
  PA_GROW_UP,
  PA_MESH,
  PA_SCROLL_UP_RIGHT,
  //PA_BLINDS,
  PA_CLOSING,
  PA_RANDOM,
  PA_GROW_DOWN,
  PA_SCAN_VERT,
  PA_SCROLL_DOWN_LEFT,
  PA_WIPE_CURSOR,
  //PA_DISSOLVE,
  PA_OPENING,
  PA_CLOSING_CURSOR,
  PA_SCROLL_DOWN_RIGHT,
  PA_SCROLL_RIGHT,
  //PA_SLICE,
  PA_SCROLL_DOWN,
};


//---------------------------------------------------------------------
int16_t calculateIntensity()
{
  int a0In = 0;
  for (int l=0; l<2; l++)
  {
    //--- read analog A0
    a0In+= analogRead(A0);
    delay(200);
  }
  a0In = a0In / 2;  //-- smooth things up a bit

  Serial.printf("analogRead[%d], ", a0In);
  //---test if (a0In < settingLDRlowOffset) a0In = settingLDRlowOffset;
  Serial.printf(" LDRlowOffset[%d] LDRhighOffset[%d] ", settingLDRlowOffset, settingLDRhighOffset);
  valueLDR = (valueLDR + a0In) / 2;
  if (valueLDR < settingLDRlowOffset)   valueLDR = settingLDRlowOffset;
  if (valueLDR > settingLDRhighOffset)  valueLDR = settingLDRhighOffset;
  Serial.printf(" ==> valueLDR[%d]\r\n", valueLDR);

  //--- map LDR to offset..1024 -> 0..settingMax
  int intensity = map(valueLDR, settingLDRlowOffset, settingLDRhighOffset,  0, settingMaxIntensity);

  return intensity;

} // calculateIntensity()


//---------------------------------------------------------------------
char *updateTime()
{
  time(&now);
  snprintf(timeMsg, 20, "%02d : %02d", localtime(&now)->tm_hour, localtime(&now)->tm_min);
  return timeMsg;

} // updateTime()


//---------------------------------------------------------------------
bool getTheLocalTime(struct tm *info, uint32_t ms)
{
  //-- getLocalTime() is not implemented in the ArduinoIDE
  //-- so this is a 'work around' function
  uint32_t start = millis();
  time_t now;
  while((millis()-start) <= ms)
  {
    time(&now);
    localtime_r(&now, info);
    if(info->tm_year > (2016 - 1900))
    {
      return true;
    }
    delay(10);
  }
  return false;

} // getTheLocalTime()


//---------------------------------------------------------------------
void nextNieuwsBericht()
{
  bool breakOut  = false;
  newsMsgID++;
  if (newsMsgID >= settingNewsMaxMsg) newsMsgID = 0;
  while (!readFileById("NWS", newsMsgID))
  {
    Serial.println("File not found!");
    newsMsgID++;
    if (newsMsgID > settingNewsMaxMsg)
    {
      newsMsgID = 0;
      breakOut  = true;
      break;
    }
  }
  if (!breakOut)
  {
    snprintf(actMessage, NEWS_SIZE, "** %s **", fileMessage);
    snprintf(onTickerMessage, 120, "%s", actMessage);
    Serial.printf("newsMsgID[%d] %s\r\n", newsMsgID, actMessage);
    TelnetStream.printf("newsMsgID[%d] %s\r\n", newsMsgID, actMessage);
    utf8Ascii(actMessage);
    P.displayScroll(actMessage, PA_LEFT, PA_SCROLL_LEFT, (MAX_SPEED - settingTextSpeed));
  }

} // nextNieuwsBericht()


//---------------------------------------------------------------------
void nextLocalBericht()
{
  bool nothingThere = false;

  localMsgID++;
  if (localMsgID > settingLocalMaxMsg)
  {
    localMsgID    = 0;
    nothingThere  = true;
  }
  while (!readFileById("LCL", localMsgID))
  {
    Serial.printf("File [/newsFiles/LCL-%03d] not found!\r\n", localMsgID);
    localMsgID++;
    if (localMsgID > settingLocalMaxMsg)
    {
      Serial.println("Back to LCL-000, exit while-loop");
      localMsgID = 0;
      continue;
    }
  }
  if (nothingThere && (localMsgID == 0))
  {
    nothingThere = true;
    getRevisionData(_FW_VERSION);
  }
  else  nothingThere = false;

  snprintf(actMessage, LCL_SIZE, "** %s **", fileMessage);
  snprintf(onTickerMessage, 120, "%s", actMessage);
  TelnetStream.printf("localMsgID[%d] %s\r\n", localMsgID, actMessage);
  utf8Ascii(actMessage);
  P.displayScroll(actMessage, PA_LEFT, PA_SCROLL_LEFT, (MAX_SPEED - settingTextSpeed));

  if ((millis() - revisionTimer) > 900000)
  {
    revisionTimer = millis();
    getRevisionData(_FW_VERSION);
  }

} // nextLocalBericht()


//=====================================================================
void setup()
{
  Serial.begin(115200);
  while(!Serial) { /* wait a bit */ }

  lastReset     = ESP.getResetReason();

  startTelnet();

  Serial.println("\r\n[MD_Parola WiFi Message Display]\r\n");
  Serial.printf("Booting....[%s]\r\n\r\n", String(_FW_VERSION).c_str());
  TelnetStream.printf("Booting....[%s]\r\n\r\n", String(_FW_VERSION).c_str());

  sprintf(actMessage, "[%s]  Booting . . . . . . . . .  ", String(_FW_VERSION).c_str());

  P.begin();
  P.displayClear();
  P.displaySuspend(false);
  P.setIntensity(2);
  P.displayScroll(actMessage, PA_LEFT, PA_NO_EFFECT, 20);
  P.setTextEffect(PA_SCROLL_LEFT, PA_NO_EFFECT);
  do
  {
    yield();
  } while( !P.displayAnimate() );

  actMessage[0]  = '\0';

  //================ LittleFS ===========================================
  if (LittleFS.begin())
  {
    Serial.println(F("LittleFS Mount succesfull\r"));
    LittleFSmounted = true;

    readSettings(true);
    splitNewsNoWords(settingNewsNoWords);

    if (settingNewsInterval == 0)
    {
      removeNewsData();
    }
    else
    {
      if (!LittleFS.exists("/newsFiles/LCL-000"))
      {
        char LCL000[100];
        sprintf(LCL000, "ESP_ticker %s by Willem Aandewiel", String(_FW_VERSION).c_str());
        writeFileById("LCL", 0, LCL000);
      }
      if (!LittleFS.exists("/newsFiles/LCL-001"))
      {
        char LCL001[100];
        sprintf(LCL001, "ESP_ticker %s by Willem Aandewiel", String(_FW_VERSION).c_str());
        writeFileById("LCL", 1, LCL001);
      }
      writeFileById("NWS", 1, "(c) 2021 Willem Aandewiel");
    }
  }
  else
  {
    Serial.println(F("LittleFS Mount failed\r"));   // Serious problem with LittleFS
    LittleFSmounted = false;
  }
  //==========================================================//
  // writeLastStatus();  // only for firsttime initialization //
  //==========================================================//
  readLastStatus(); // place it in actTimestamp

  // attempt to connect to Wifi network:
  int t = 0;
  Serial.println("Attempting to connect to WiFi network ");
  //P.displayClear();
  P.displaySuspend(false);
  P.displayScroll("connect to WiFi", PA_LEFT, PA_NO_EFFECT, 25);
  P.setTextEffect(PA_SCROLL_LEFT, PA_NO_EFFECT);
  do
  {
    yield();
  } while( !P.displayAnimate() );

  // Connect to and initialise WiFi network
  digitalWrite(LED_BUILTIN, HIGH);
  startWiFi(_HOSTNAME, 240, &httpServer);  // timeout 4 minuten

  // Set up first message as the IP address
  sprintf(actMessage, "%03d.%03d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
  Serial.printf("\nAssigned IP[%s]\r\n", actMessage);
  P.displayClear();
  P.displaySuspend(false);
  P.displayScroll(actMessage, PA_LEFT, PA_NO_EFFECT, 25);
  P.setTextEffect(PA_SCROLL_LEFT, PA_NO_EFFECT);
  do
  {
    yield();
  } while( !P.displayAnimate() );

  digitalWrite(LED_BUILTIN, LOW);

  startMDNS(settingHostname);

  Serial.println("Get time from NTP");
  timeSync.setup();
  timeSync.sync(100);
  time(&now);
  if (localtime(&now)->tm_year > 120)
  {
    timeSynced = true;
    Serial.println("Time synchronized with NTP Service");
  }
  else
  {
    timeSynced = false;
    Serial.println("Could not synchronize time with NTP Service");
  }

  Serial.printf("\nAssigned IP[%s]\r\n", actMessage);

  time(&now);
  Serial.println("-------------------------------------------------------------------------------");
  if (!getTheLocalTime(&timeinfo, 10000))
  {
    Serial.println("Time       : Failed to obtain time!");
  }
  else
  {
    Serial.printf( "Time       : %04d-%02d-%02d %02d:%02d:%02d\r\n", localtime(&now)->tm_year+1900
                   , localtime(&now)->tm_mon+1
                   , localtime(&now)->tm_mday
                   , localtime(&now)->tm_hour
                   , localtime(&now)->tm_min
                   , localtime(&now)->tm_sec);
  }

  nrReboots++;
  writeLastStatus();

  Serial.flush();
  snprintf(cMsg, sizeof(cMsg), "Last reset reason: [%s]", ESP.getResetReason().c_str());
  Serial.println(cMsg);
  TelnetStream.println(cMsg);

  Serial.flush();
  Serial.print("\nGebruik 'telnet ");
  Serial.print (WiFi.localIP());
  Serial.println("' voor verdere logging ...\r\n");
  Serial.flush();

  weerlive.setup(settingWeerLiveAUTH, settingWeerLiveLocation);
  Serial.flush();

  //================ Start HTTP Server ================================
  setupFSexplorer();
  httpServer.serveStatic("/FSexplorer.png",   LittleFS, "/FSexplorer.png");
  httpServer.on("/",          sendIndexPage);
  httpServer.on("/index",     sendIndexPage);
  httpServer.on("/index.html", sendIndexPage);
  httpServer.serveStatic("/index.css", LittleFS, "/index.css");
  httpServer.serveStatic("/index.js",  LittleFS, "/index.js");
  // all other api calls are catched in FSexplorer onNotFounD!
  httpServer.on("/api", HTTP_GET, processAPI);


  httpServer.begin();
  Serial.println("\nServer started\r\n");

  valueIntensity = calculateIntensity(); // read analog input pin 0

  P.setIntensity(valueIntensity);
  newsMsgID = 0;
  do
  {
    yield();
  } while( !P.displayAnimate() );

  P.setFont(ExtASCII);

  inFX = 0;
  outFX= 0;

  snprintf(actMessage, NEWS_SIZE, "ESP_ticker by Willem Aandewiel              Wait ...     ");
  utf8Ascii(actMessage);
  P.displayScroll(actMessage, PA_LEFT, PA_SCROLL_LEFT, (MAX_SPEED - settingTextSpeed));
  valueIntensity = calculateIntensity(); // read analog input pin 0
  P.setIntensity(valueIntensity);
  P.displayClear();
  P.displaySuspend(false);
  P.displayScroll(actMessage, PA_LEFT, PA_NO_EFFECT, 25);
  P.setTextEffect(PA_SCROLL_LEFT, PA_NO_EFFECT);

  do
  {
    yield();
  } while( !P.displayAnimate() );

  //-- clear all NWS files --
  for (int i=0; i<=settingNewsMaxMsg; i++)
  {
    writeFileById("NWS", i, "");
  }

} // setup()


//=====================================================================
void loop()
{
  httpServer.handleClient();
  MDNS.update();
  yield();

  if ((millis() > weerTimer) && (strlen(settingWeerLiveAUTH) > 5))
  {
    weerTimer = millis() + (settingWeerLiveInterval * (60 * 1000)); // Interval in Minutes!
    if (settingWeerLiveInterval > 0)
    {
      const char *weatherInfo = weerlive.request();
      Serial.println(weatherInfo);
      snprintf(lastWeerMessage, WEER_SIZE, "%s", weatherInfo);
      Serial.printf("ticker Weer\n%s\r\n", lastWeerMessage);
      TelnetStream.printf("ticker Weer[%s]\r\n", lastWeerMessage);
    }
  }

  if ((millis() > newsapiTimer) && (strlen(settingNewsAUTH) > 5))
  {
    newsapiTimer = millis() + (settingNewsInterval * (60 * 1000)); // Interval in Minutes!
    if (settingNewsInterval > 0)
    {
      if (!getNewsapiData())  //-- first try ...
      {
        delay(100);
        if (!getNewsapiData())  //-- second try ...
        {
          //-- try again in two(2) minutes ...
          newsapiTimer = millis() + (2 * (60 * 1000)); // Interval in Minutes!
        }
      }
    }
  }

  //-- done with animation, ready for next message
  if (P.displayAnimate())
  {
    yield();
    msgType++;
    Serial.printf("msgType[%d]\r\n", msgType);
    time(&now);
    if (localtime(&now)->tm_year > 120) timeSynced = true;

    switch(msgType)
    {
      case 1:
        if (!(millis() > timeTimer))  return;
        if (!timeSynced)              return;
        inFX  = random(0, ARRAY_SIZE(effect));
        outFX = random(0, ARRAY_SIZE(effect));
        snprintf(actMessage, LCL_SIZE, weekDayName[localtime(&now)->tm_wday+1]);
        snprintf(onTickerMessage, 120, "%s", actMessage);
        P.displayClear();
        P.displayText(actMessage, PA_CENTER, (MAX_SPEED - settingTextSpeed), 1000, effect[inFX], effect[outFX]);
        Serial.printf("[1] %s\r\n", actMessage);
        TelnetStream.printf("%s ", actMessage);
        break;
      case 2:
        if (!(millis() > timeTimer))  return;
        if (!timeSynced)              return;
        timeTimer = millis() + 60000;
        inFX  = random(0, ARRAY_SIZE(effect));
        outFX = random(0, ARRAY_SIZE(effect));
        sprintf(actMessage, "%s", updateTime());
        snprintf(onTickerMessage, 120, "%s", actMessage);
        P.displayText(actMessage, PA_CENTER, (MAX_SPEED - settingTextSpeed), 2000, effect[inFX], effect[outFX]);
        Serial.printf("[2] %s\r\n", actMessage);
        TelnetStream.printf("[%s]\r\n", actMessage);
        break;
      case 3:
        nextLocalBericht();
        P.setTextEffect(PA_SCROLL_LEFT, PA_NO_EFFECT);
        break;
      case 6:
        nextLocalBericht();
        P.setTextEffect(PA_SCROLL_LEFT, PA_NO_EFFECT);
        break;
      case 4:
      case 5:
      case 7:
      case 8:
        if (settingNewsInterval > 0)
          nextNieuwsBericht();
        else  nextLocalBericht();
        P.setTextEffect(PA_SCROLL_LEFT, PA_NO_EFFECT);
        break;
      case 9:
        if (settingWeerLiveInterval > 0)
        {
          snprintf(actMessage, WEER_SIZE, "** %s **", lastWeerMessage);
          snprintf(onTickerMessage, 120, "%s", actMessage);
          Serial.printf("WeerLive [%s]\r\n", actMessage);
          TelnetStream.printf("WeerLive [%s]\r\n", actMessage);
          utf8Ascii(actMessage);
        }
        else  nextLocalBericht();
        P.setTextEffect(PA_SCROLL_LEFT, PA_NO_EFFECT);
        break;
      case 10:
        if (settingNewsInterval > 0)
          nextNieuwsBericht();
        else  nextLocalBericht();
        break;
      default:
        msgType = 0;
        return;

    } // switch()

    valueIntensity = calculateIntensity(); // read analog input pin 0
    Serial.printf("Intensity set to [%d]\r\n", valueIntensity);
    P.setIntensity(valueIntensity);
    //-- Tell Parola we have a new animation
    P.displayReset();
    Serial.println("End of displayAnimate()..");

  } // dislayAnimate()


} // loop()


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