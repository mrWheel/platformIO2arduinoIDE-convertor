#ifndef SETTINGSSTUFF_H
#define SETTINGSSTUFF_H

#include <Arduino.h>
#include <LittleFS.h>
#include "allDefines.h"
#include "helperStuff.h"
#include "networkStuff.h"
#include "newsapi_org.h"

// Function Prototypes
void writeSettings(bool show);
void readSettings(bool show);
void updateSetting(const char *field, const char *newValue);

// External global variables
extern char       settingHostname[];
extern char       settingNewsNoWords[];
extern uint8_t    settingLocalMaxMsg;
extern uint8_t    settingTextSpeed;
extern uint16_t   settingLDRlowOffset;
extern uint16_t   settingLDRhighOffset;
extern uint8_t    settingMaxIntensity;
extern char       settingWeerLiveAUTH[];
extern char       settingWeerLiveLocation[];
extern uint8_t    settingWeerLiveInterval;
extern char       settingNewsAUTH[];
extern uint8_t    settingNewsMaxMsg;
extern uint8_t    settingNewsInterval;
extern bool       Verbose;
extern char       tempMessage[];

#endif // SETTINGSSTUFF_H
