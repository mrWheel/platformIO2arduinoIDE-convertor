#ifndef NETWORKSTUFF_H
#define NETWORKSTUFF_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <WiFiManager.h>
#include <FS.h>
#include <LittleFS.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <TelnetStream.h>

#include "allDefines.h"
#include "updateServerHtml.h"

#ifdef USE_UPDATE_SERVER
  //#include "ESP8266HTTPUpdateServer.h"
  #include <ModUpdateServer.h>   // https://github.com/mrWheel/ModUpdateServer
  #include "updateServerHtml.h"
#endif
#include <WiFiManager.h>       // version 0.15.0 - https://github.com/tzapu/WiFiManager
//#include <FS.h>                // part of ESP8266 Core https://github.com/esp8266/Arduino


// External global variables
extern ESP8266WebServer httpServer;
#ifdef USE_UPDATE_SERVER
  extern ESP8266HTTPUpdateServer httpUpdater;
#endif
//extern FSInfo LittleFSinfo;
extern bool LittleFSmounted;
extern bool isConnected;
extern MD_Parola P;

// Function prototypes
void configModeCallback(WiFiManager *myWiFiManager);
void startWiFi(const char* hostname, int timeOut, ESP8266WebServer *server);
void startTelnet();
void startMDNS(const char *Hostname);

// Update server HTML content (assuming they are defined elsewhere in your project)
extern const char UpdateServerIndex[];
extern const char UpdateServerSuccess[];

#endif // NETWORKSTUFF_H
