/*
***************************************************************************  
**  Program : WeerliveClass.h
**
**  Copyright (c) 2024 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/
#ifndef WEERLIVE_H
#define WEERLIVE_H

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>                    // JSON decoding library
//#include <TimeLib.h>

class Weerlive 
{
  public:
      Weerlive(WiFiClient &weerliveClient);
      void setup(const char *key, const char *city);
      const char *request();

  private:
      WiFiClient         &thisClient;
      String              apiUrl;
      static const char  *apiHost;
      String              weerliveText;
      char                jsonResponse[2000];
      int                 alarmInd = -1;
      const char        *dateToDayName(const char* date_str);

      //-- if the payload string is to large you will get a deserial error
      StaticJsonDocument<8000> doc;
      //-- if you select to many fields you will get a deserial error
      StaticJsonDocument<800> filter;

      void configureFilters();
};

#endif // WEERLIVE_H

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
