/*
***************************************************************************
**  Program : newsapi_org
**
**  Copyright (c) 2021 .. 2024 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
*/

#include "newsapi_org.h"

//-- http://newsapi.org/v2/top-headlines?country=nl&apiKey=API_KEY


//----------------------------------------------------------------------
/**
 * Retrieves news data from newsapi.org and processes it accordingly.
 *
 * @return true if the news data retrieval and processing was successful,
 *         false otherwise
 *
 * @throws None
 */
bool getNewsapiData()
{
  const char *newsapiHost    = "newsapi.org";
  const int   httpPort       = 80;
  int         newsapiStatus  = 0;
  char        newsMessage[NEWS_SIZE] = {};
  int         startPos, endPos;
  int32_t     maxWait;
  char        jsonResponse[JSON_RESPONSE_SIZE] = {};
  char        val[51] = "";

  WiFiClient newsapiClient;

  Serial.println();
  Serial.printf("getNewsapiData(%s)\r\n", newsapiHost);

  // We now create a URI for the request
  String url = "/v2/top-headlines?country=nl&apiKey=";
  url += settingNewsAUTH;

  Serial.println("\r\n=================================================");
  Serial.printf("Requesting URL: %s/v2/top-headlines?country=nl&apiKey=secret\r\n", newsapiHost);
  Serial.println("=================================================");
  Serial.flush();

  if (!newsapiClient.connect(newsapiHost, httpPort))
  {
    sprintf(tempMessage, "connection to %s failed", newsapiHost);
    Serial.println(tempMessage);
    //-- empty newsMessage store --
    for(int i=0; i<=settingNewsMaxMsg; i++)
    {
      //sprintf(newsMessage, "");
      if (i==1) writeFileById("NWS", i, "There is No News ....");
      else      writeFileById("NWS", i, "");
    }

    newsapiClient.flush();
    newsapiClient.stop();
    return false;
  }

  // This will send the request to the server
  Serial.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + newsapiHost + "\r\n" +
               "User-Agent: ESP-ticker\r\n" +
               "Connection: close\r\n\r\n");
  newsapiClient.print(String("GET ") + url + " HTTP/1.1\r\n" +
                      "Host: " + newsapiHost + "\r\n" +
                      "User-Agent: ESP-ticker\r\n" +
                      "Connection: close\r\n\r\n");
  delay(10);

  newsapiClient.setTimeout(5000);
  while (newsapiClient.connected() || newsapiClient.available())
  {
    yield();
    while(newsapiClient.available())
    {
      Serial.println();
      //--- skip to find HTTP/1.1
      //--- then parse response code
      if (newsapiClient.find("HTTP/1.1"))
      {
        newsapiStatus = newsapiClient.parseInt(); // parse status code
        Serial.printf("newsApi Statuscode: [%d] ", newsapiStatus);
        if (newsapiStatus != 200)
        {
          Serial.println(" ERROR!");
          while(newsapiClient.available())
          {
            char nC = newsapiClient.read();
            if (nC >= 32 && nC <= 126) Serial.print(nC);
          }
          Serial.println();
          newsapiClient.flush();
          newsapiClient.stop();
          for(int i=0; i<=settingNewsMaxMsg; i++)
          {
            //sprintf(newsMessage, "");
            if (i==1)
              writeFileById("NWS", i, "There is No News ....");
            else if (i==1 && newsapiStatus == 429)
              writeFileById("NWS", i, "You have made too many news requests recently . . please try again later");
            else
            {
              writeFileById("NWS", i, "");
            }
          }
          newsapiClient.flush();
          newsapiClient.stop();

          return false;
        }
        Serial.println(" OK!");
      }
      else
      {
        Serial.println("Error reading newsapi.org.. -> bailout!");
        for(int i=0; i<=settingNewsMaxMsg; i++)
        {
          //sprintf(newsMessage, "");
          if (i==1) writeFileById("NWS", i, "There is No News ....");
          else
          {
            writeFileById("NWS", i, "");
          }
        }
        newsapiClient.flush();
        newsapiClient.stop();
        return false;
      }
      //--- skip headers
      uint16_t  msgIdx  = 0;
      int       msgNr   = 0;
      while (newsapiClient.find("\"title\":\""))
      {
        for (int i=0; i<NEWS_SIZE; i++) newsMessage[i] = 0;
        msgIdx          = 0;
        while( (strIndex(newsMessage, "\"description\":") == -1) && (msgIdx < (NEWS_SIZE - 2)) )
        {
          char cN = newsapiClient.read();
          if (cN >= 32 && cN <= 126) newsMessage[msgIdx++] = cN;
        }
        newsMessage[msgIdx] = '\0';
        //-- what happens here?? I don't know anymore :-()
        if (msgIdx > 30)  newsMessage[msgIdx - 16] = 0;
        Serial.printf("\t[%2d] %s\r\n", msgNr, newsMessage);
        if (!hasNoNoWord(newsMessage) && strlen(newsMessage) > 15)
        {
          if (msgNr <= settingNewsMaxMsg)
          {
            writeFileById("NWS", msgNr, newsMessage);
          }
          msgNr++;
        }
      } // while find(title)
    } // while available ..

  } // connected ..

  newsapiClient.flush();
  newsapiClient.stop();
  updateMessage("0", "News brought to you by 'newsapi.org'");

  return true;

} // getNewsapiData()


//----------------------------------------------------------------------
/**
 * Removes news data from LittleFS for a given range of news messages.
 *
 * @return void
 */
void removeNewsData()
{
  char nwsName[32];

  for(int n=0; n<=settingNewsMaxMsg; n++)
  {
    sprintf(nwsName, "/newsFiles/NWS-%03d", n);
    Serial.printf("Remove [%s] from LittleFS ..\r\n", nwsName);
    LittleFS.remove(nwsName);
  }

} //  removeNewsData()

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