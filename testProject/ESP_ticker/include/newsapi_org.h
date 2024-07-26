#ifndef NEWSAPI_ORG_H
#define NEWSAPI_ORG_H

#include <Arduino.h>
#include <WiFiClient.h>
#include <LittleFS.h>
#include <TelnetStream.h>
#include "allDefines.h"
#include "helperStuff.h"
#include "littlefsStuff.h"

// Function prototypes
bool getNewsapiData();
void removeNewsData();

// External global variables
extern char      settingNewsAUTH[];
extern char      settingNewsNoWords[];
extern uint8_t   settingLocalMaxMsg, settingTextSpeed, settingMaxIntensity;
extern uint16_t  settingLDRlowOffset, settingLDRhighOffset;
extern char      settingWeerLiveAUTH[], settingWeerLiveLocation[];
extern uint8_t   settingWeerLiveInterval;
extern uint8_t   settingNewsInterval, settingNewsMaxMsg;

extern const int httpPort;
extern int newsapiStatus;
extern char newsMessage[];
extern int32_t maxWait;
extern char jsonResponse[];
extern char onTickerMessage[];
extern char val[];
extern WiFiClient newsapiClient;

#endif // NEWSAPI_ORG_H
