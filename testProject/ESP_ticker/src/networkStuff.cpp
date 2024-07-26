/*
***************************************************************************
**  Program : networkStuff.h
**
**  Copyright (c) 2021 .. 2024 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
**      Usage:
**
**      #define HOSTNAME  thisProject
**
**      setup()
**      {
**        startTelnet();
**        startWiFi(_HOSTNAME, 240);  // timeout 4 minuten
**        startMDNS(_HOSTNAME);
**        httpServer.on("/index",     <sendIndexPage>);
**        httpServer.on("/index.html",<sendIndexPage>);
**        httpServer.begin();
**      }
**
**      loop()
**      {
**        handleWiFi();
**        MDNS.update();
**        httpServer.handleClient();
**        .
**        .
**      }
*/

#include "networkStuff.h"

//===========================================================================================
/**
 * Callback function that is called when the WiFiManager enters configuration mode.
 *
 * @param myWiFiManager Pointer to the WiFiManager object.
 *
 * @return void
 *
 * @throws None
 */
void configModeCallback (WiFiManager *myWiFiManager)
{
  Serial.println(F("Entered config mode\r"));
  Serial.println(WiFi.softAPIP().toString());
  P.displayClear();
  P.displayScroll("Enter WiFi configuration mode", PA_LEFT, PA_NO_EFFECT, 25);
  P.setTextEffect(PA_SCROLL_LEFT, PA_NO_EFFECT);
  do
  {
    yield();
  } while( !P.displayAnimate() );
  delay(5000);
  P.displayClear();
  P.displayScroll("Connect to AP ESPticker-xx:yy", PA_LEFT, PA_NO_EFFECT, 25);
  P.setTextEffect(PA_SCROLL_LEFT, PA_NO_EFFECT);
  do
  {
    yield();
  } while( !P.displayAnimate() );
  delay(5000);
  P.displayClear();
  P.displayScroll("and brows to    192.168.4.1", PA_LEFT, PA_NO_EFFECT, 25);
  P.setTextEffect(PA_SCROLL_LEFT, PA_NO_EFFECT);
  do
  {
    yield();
  } while( !P.displayAnimate() );

  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());

} // configModeCallback()


//===========================================================================================
/**
 * Initializes WiFi connection and configuration portal.
 *
 * @param hostname The hostname for the WiFi network.
 * @param timeOut Timeout value for configuration portal.
 * @param server Pointer to the ESP8266WebServer object.
 *
 * @return void
 *
 * @throws None
 */
void startWiFi(const char *hostname, int timeOut, ESP8266WebServer *server)
{
  WiFiManager manageWiFi;
  uint32_t lTime = millis();
  String thisAP = String(hostname) + "-" + WiFi.macAddress();

  Serial.print("start ...");

  manageWiFi.setDebugOutput(false);

  //--- set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  manageWiFi.setAPCallback(configModeCallback);

  //--- sets timeout until configuration portal gets turned off
  //--- useful to make it all retry or go to sleep in seconds
  //manageWiFi.setTimeout(240);  // 4 minuten
  manageWiFi.setTimeout(timeOut);  // in seconden ...

  //--- fetches ssid and pass and tries to connect
  //--- if it does not connect it starts an access point with the specified name
  //--- here  "lichtKrant-<MAC>"
  //--- and goes into a blocking loop awaiting configuration
  if (!manageWiFi.autoConnect(thisAP.c_str()))
  {
    Serial.println(F("failed to connect and hit timeout"));

    Serial.printf(" took [%d] seconds ==> ERROR!\r\n", (millis() - lTime) / 1000);
    return;
  }

  Serial.println();
  Serial.print(F("Connected to " ));
  Serial.println (WiFi.SSID());
  Serial.print(F("IP address: " ));
  Serial.println (WiFi.localIP());
  Serial.print(F("IP gateway: " ));
  Serial.println (WiFi.gatewayIP());
  Serial.println();

  #ifdef USE_UPDATE_SERVER
    httpUpdater.setup(server);
    httpUpdater.setIndexPage(UpdateServerIndex);
    httpUpdater.setSuccessPage(UpdateServerSuccess);
  #endif // USE_UPDATE_SERVER

  Serial.printf(" took [%d] seconds => OK!\r\n", (millis() - lTime) / 1000);

} // startWiFi()


//===========================================================================================
/**
 * Starts the Telnet server and prints a message indicating that the server has started.
 *
 * @return void
 */
void startTelnet()
{
  TelnetStream.begin();
  Serial.println(F("\nTelnet server started .."));
  TelnetStream.flush();

} // startTelnet()


//=======================================================================
/**
 * Starts the mDNS responder with the specified hostname.
 *
 * @param Hostname The hostname to be used for the mDNS responder.
 *
 * @return void
 *
 * @throws None
 */
void startMDNS(const char *Hostname)
{
  Serial.printf("[1] mDNS setup as [%s.local]\r\n", Hostname);
  if (MDNS.begin(Hostname))               // Start the mDNS responder for Hostname.local
  {
    Serial.printf("[2] mDNS responder started as [%s.local]\r\n", Hostname);
  }
  else
  {
    Serial.println(F("[3] Error setting up MDNS responder!\r\n"));
  }
  MDNS.addService("http", "tcp", 80);

} // startMDNS()

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