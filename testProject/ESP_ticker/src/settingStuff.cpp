/*
***************************************************************************
**  Program  : settingsStuff, part of ESP_ticker
**
**  Copyright (c) 2021 .. 2024 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
*/

#include "settingStuff.h"

//=======================================================================
/**
 * Writes the setting data to a file including various parameters such as
 * Hostname, newsNoWords, localMaxMsg, textSpeed, LDRlowOffset,
 * LDRhighOffset, maxIntensity, weerLiveAUTH, weerLiveLocatie,
 * weerLiveInterval, newsAUTH, newsNoWords, newsMaxMsg, and newsInterval.
 *
 * @param show Flag indicating whether to display the written settings.
 *
 * @throws No specific error is mentioned.
 */
void writeSettings(bool show)
{
  Serial.printf("Writing to [%s] ..\r\n", SETTINGS_FILE);
  File file = LittleFS.open(SETTINGS_FILE, "w"); // open for reading and writing
  if (!file)
  {
    Serial.printf("open(%s, 'w') FAILED!!! --> Bailout\r\n", SETTINGS_FILE);
    return;
  }
  yield();

  Serial.print(F("Start writing setting data "));

  file.print("Hostname = ");
  file.println(settingHostname);
  Serial.print(F("."));
  file.print("localMaxMsg = ");
  file.println(settingLocalMaxMsg);
  Serial.print(F("."));
  file.print("textSpeed = ");
  file.println(settingTextSpeed);
  Serial.print(F("."));
  file.print("maxIntensity = ");
  file.println(settingMaxIntensity);
  Serial.print(F("."));
  file.print("LDRlowOffset = ");
  file.println(settingLDRlowOffset);
  Serial.print(F("."));
  file.print("LDRhighOffset = ");
  file.println(settingLDRhighOffset);
  Serial.print(F("."));
  file.print("weerLiveAUTH = ");
  file.println(settingWeerLiveAUTH);
  Serial.print(F("."));
  file.print("weerLiveLocatie = ");
  file.println(settingWeerLiveLocation);
  Serial.print(F("."));
  file.print("weerLiveInterval = ");
  file.println(settingWeerLiveInterval);
  Serial.print(F("."));
  file.print("newsAUTH = ");
  file.println(settingNewsAUTH);
  Serial.print(F("."));
  file.print("newsNoWords = ");
  file.println(settingNewsNoWords);
  Serial.print(F("."));
  file.print("newsMaxMsg = ");
  file.println(settingNewsMaxMsg);
  Serial.print(F("."));
  file.print("newsInterval = ");
  file.println(settingNewsInterval);
  Serial.print(F("."));

  file.close();

  Serial.println(F(" done"));

  if (show)
  {
    Serial.println(F("Wrote this:"));
    Serial.print(F("        Hostname = "));
    Serial.println(settingHostname);
    Serial.print(F("     newsNoWords = "));
    Serial.println(settingNewsNoWords);
    Serial.print(F("     localMaxMsg = "));
    Serial.println(settingLocalMaxMsg);
    Serial.print(F("       textSpeed = "));
    Serial.println(settingTextSpeed);
    Serial.print(F("    LDRlowOffset = "));
    Serial.println(settingLDRlowOffset);
    Serial.print(F("   LDRhighOffset = "));
    Serial.println(settingLDRhighOffset);
    Serial.print(F("    maxIntensity = "));
    Serial.println(settingMaxIntensity);
    Serial.print(F("    weerLiveAUTH = "));
    Serial.println(settingWeerLiveAUTH);
    Serial.print(F(" weerLiveLocatie = "));
    Serial.println(settingWeerLiveLocation);
    Serial.print(F("weerLiveInterval = "));
    Serial.println(settingWeerLiveInterval);
    Serial.print(F("        newsAUTH = "));
    Serial.println(settingNewsAUTH);
    Serial.print(F("      newsMaxMsg = "));
    Serial.println(settingNewsMaxMsg);
    Serial.print(F("    newsInterval = "));
    Serial.println(settingNewsInterval);

  } // Verbose

} // writeSettings()


//=======================================================================
/**
 * Reads and processes settings from a file, updates the global variables
 * accordingly, and optionally displays the settings.
 *
 * @param show a boolean indicating whether to display the settings
 *
 * @throws None
 */
void readSettings(bool show)
{
  String sTmp;
  String words[10];
  char cTmp[LCL_SIZE], cVal[101], cKey[101];

  File file;

  Serial.printf(" %s ..\r\n", SETTINGS_FILE);

  snprintf(settingHostname,    HOSTNAME_SIZE, "%s", _HOSTNAME);
  snprintf(settingNewsNoWords, LCL_SIZE, "Voetbal, show, UEFA, KNVB");
  settingLocalMaxMsg        =   5;
  settingTextSpeed          =  25;
  settingLDRlowOffset       =  70;
  settingLDRhighOffset      = 700;
  settingMaxIntensity       =   6;
  snprintf(settingWeerLiveAUTH,  WEER_AUTH_SIZE, "");
  snprintf(settingWeerLiveLocation, WEER_LIVE_LOC_SIZE, "");
  settingWeerLiveInterval   =   0;
  snprintf(settingNewsAUTH,  NEWS_AUTH_SIZE, "");
  settingNewsMaxMsg         =   4;
  settingNewsInterval       =   0;

  if (!LittleFS.exists(SETTINGS_FILE))
  {
    Serial.println(F(" .. file not found! --> created file!"));
    writeSettings(show);
  }

  for (int T = 0; T < 2; T++)
  {
    file = LittleFS.open(SETTINGS_FILE, "r");
    if (!file)
    {
      if (T == 0) Serial.printf(" .. something went wrong opening [%s]\r\n", SETTINGS_FILE);
      else        Serial.print(T);
      delay(100);
    }
  } // try T times ..

  Serial.println(F("Reading settings:\r"));
  while(file.available())
  {
    sTmp = file.readStringUntil('\n');
    snprintf(cTmp, sizeof(cTmp), "%s", sTmp.c_str());
    strTrimCntr(cTmp, sizeof(cTmp));
    int sEq = strIndex(cTmp, "=");
    strCopy(cKey, 100, cTmp, 0, sEq -1);
    strCopy(cVal, 100, cTmp, sEq +1, strlen(cTmp));
    strTrim(cKey, sizeof(cKey), ' ');
    strTrim(cVal, sizeof(cVal), ' ');

    //strToLower(cKey);
    if (stricmp(cKey, "hostname") == 0)         strCopy(settingHostname,         HOSTNAME_SIZE, cVal);
    if (stricmp(cKey, "localMaxMsg") == 0)      settingLocalMaxMsg      = atoi(cVal);
    if (stricmp(cKey, "textSpeed") == 0)        settingTextSpeed        = atoi(cVal);
    if (stricmp(cKey, "LDRlowOffset") == 0)     settingLDRlowOffset     = atoi(cVal);
    if (stricmp(cKey, "LDRhighOffset") == 0)    settingLDRhighOffset    = atoi(cVal);
    if (stricmp(cKey, "maxIntensity") == 0)     settingMaxIntensity     = atoi(cVal);
    if (stricmp(cKey, "weerLiveAUTH") == 0)     strCopy(settingWeerLiveAUTH,     WEER_AUTH_SIZE, cVal);
    if (stricmp(cKey, "weerlivelocatie") == 0)  strCopy(settingWeerLiveLocation, WEER_LIVE_LOC_SIZE, cVal);
    if (stricmp(cKey, "weerLiveInterval") == 0) settingWeerLiveInterval = atoi(cVal);
    if (stricmp(cKey, "newsAUTH") == 0)         strCopy(settingNewsAUTH,         NEWS_AUTH_SIZE, cVal);
    if (stricmp(cKey, "newsNoWords") == 0)      strCopy(settingNewsNoWords,      LCL_SIZE, cVal);
    if (stricmp(cKey, "newsMaxMsg") == 0)       settingNewsMaxMsg       = atoi(cVal);
    if (stricmp(cKey, "newsInterval") == 0)     settingNewsInterval     = atoi(cVal);

  } // while available()

  file.close();

  //--- this will take some time to settle in
  //--- probably need a reboot before that to happen :-(
  MDNS.setHostname(settingHostname);    // start advertising with new(?) settingHostname
  if (settingLocalMaxMsg > 20)        settingLocalMaxMsg      =   20;
  if (settingLocalMaxMsg <  1)        settingLocalMaxMsg      =    1;
  if (settingTextSpeed > MAX_SPEED)   settingTextSpeed        =  MAX_SPEED;
  if (settingTextSpeed < 10)          settingTextSpeed        =   10;
  if (settingLDRlowOffset  >  500)    settingLDRlowOffset     =  500;
  if (settingLDRlowOffset  <    1)    settingLDRlowOffset     =    0;
  if (settingLDRhighOffset <  500)    settingLDRhighOffset    =  500;
  if (settingLDRhighOffset > 1024)    settingLDRhighOffset    = 1024;
  if (settingMaxIntensity > 15)       settingMaxIntensity     =   15;
  if (settingMaxIntensity <  1)       settingMaxIntensity     =    1;
  if (strlen(settingWeerLiveLocation) <  1)  sprintf(settingWeerLiveLocation, "Amsterdam");
  if (settingWeerLiveInterval ==  0)
  {
    settingWeerLiveInterval =    0;  // geen weerberichten
  }
  else
  {
    if (settingWeerLiveInterval > 120)  settingWeerLiveInterval =  120;  // minuten!
    if (settingWeerLiveInterval <   2)  settingWeerLiveInterval =    2;
  }
  if (settingNewsMaxMsg > 20)         settingNewsMaxMsg       =   20;
  if (settingNewsMaxMsg <  1)         settingNewsMaxMsg       =    1;
  if (settingNewsInterval > 120)      settingNewsInterval     =  120;
  if (settingNewsInterval ==  0)
  {
    settingNewsInterval  =    0; // geen nieuwsberichten
  }
  else
  {
    if (settingNewsInterval <  15)    settingNewsInterval     =   15;
  }

  Serial.println(F(" .. done\r"));

  if (!show) return;

  Serial.println(F("\r\n==== read Settings ===================================================\r"));
  Serial.printf("                 Hostname : %s\r\n",  settingHostname);
  Serial.printf("           local Max. Msg : %d\r\n",  settingLocalMaxMsg);
  Serial.printf("               text Speed : %d\r\n",  settingTextSpeed);
  Serial.printf("           LDR low offset : %d\r\n",  settingLDRlowOffset);
  Serial.printf("          LDR high offset : %d\r\n",  settingLDRhighOffset);
  Serial.printf("            max Intensity : %d\r\n",  settingMaxIntensity);
  Serial.printf("         WeerLive.nl AUTH : %s\r\n",  settingWeerLiveAUTH);
  Serial.printf("      WeerLive.nl Locatie : %s\r\n",  settingWeerLiveLocation);
  Serial.printf("     WeerLive.nl Interval : %d\r\n",  settingWeerLiveInterval);
  Serial.printf("         newsapi.org AUTH : %s\r\n",  settingNewsAUTH);
  Serial.printf("      newsapi.org NoWords : %s\r\n",  settingNewsNoWords);
  Serial.printf("     newsapi.org Max. Msg : %d\r\n",  settingNewsMaxMsg);
  Serial.printf("     newsapi.org Interval : %d\r\n",  settingNewsInterval);

  Serial.println(F("-\r"));

} // readSettings()


//=======================================================================
/**
 * Updates a setting based on the provided field and new value.
 *
 * @param field The field of the setting to update.
 * @param newValue The new value of the setting.
 *
 * @throws None
 */
void updateSetting(const char *field, const char *newValue)
{
  Serial.printf("-> field[%s], newValue[%s]\r\n", field, newValue);

  if (!stricmp(field, "Hostname"))
  {
    strCopy(settingHostname, HOSTNAME_SIZE, newValue);
    if (strlen(settingHostname) < 1) strCopy(settingHostname, HOSTNAME_SIZE, _HOSTNAME);
    char *dotPntr = strchr(settingHostname, '.') ;
    if (dotPntr != NULL)
    {
      byte dotPos = (dotPntr-settingHostname);
      if (dotPos > 0)  settingHostname[dotPos] = '\0';
    }
    Serial.println();
    Serial.printf("Need reboot before new %s.local will be available!\r\n\n", settingHostname);
  }
  if (!stricmp(field, "localMaxMsg"))      settingLocalMaxMsg   = String(newValue).toInt();
  if (!stricmp(field, "textSpeed"))        settingTextSpeed     = String(newValue).toInt();
  if (!stricmp(field, "LDRlowOffset"))     settingLDRlowOffset  = String(newValue).toInt();
  if (!stricmp(field, "LDRhighOffset"))    settingLDRhighOffset = String(newValue).toInt();
  if (!stricmp(field, "maxIntensity"))     settingMaxIntensity  = String(newValue).toInt();

  if (!stricmp(field, "weerLiveAUTH"))     strCopy(settingWeerLiveAUTH, WEER_AUTH_SIZE, newValue);
  if (!stricmp(field, "weerLiveLocation")) strCopy(settingWeerLiveLocation, WEER_LIVE_LOC_SIZE, newValue);
  if (!stricmp(field, "weerLiveInterval")) settingWeerLiveInterval  = String(newValue).toInt();

  if (!stricmp(field, "newsapiAUTH"))      strCopy(settingNewsAUTH, NEWS_AUTH_SIZE, newValue);
  if (!stricmp(field, "newsNoWords"))      strCopy(settingNewsNoWords, LCL_SIZE, newValue);
  if (!stricmp(field, "newsapiMaxMsg"))    settingNewsMaxMsg   = String(newValue).toInt();
  if (!stricmp(field, "newsapiInterval"))  settingNewsInterval = String(newValue).toInt();

  writeSettings(false);

  if (settingWeerLiveInterval == 0)      memset(tempMessage, 0, LCL_SIZE);
  else if (settingWeerLiveInterval < 2)  settingWeerLiveInterval = 2;
  if (settingNewsInterval == 0)          removeNewsData();
  else if (settingNewsInterval < 15)     settingNewsInterval = 15;
  //--- rebuild noWords array --
  splitNewsNoWords(settingNewsNoWords);

} // updateSetting()


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