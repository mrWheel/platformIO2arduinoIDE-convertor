#ifndef LITTLEFS_STUFF_H
#define LITTLEFS_STUFF_H

#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>
#include <ctime>
#include "allDefines.h"

extern char     cDate[];
extern char     cTime[];
extern uint32_t nrReboots;
extern char     fileMessage[];
extern char     tempMessage[];
extern uint8_t  settingLocalMaxMsg, settingTextSpeed, settingMaxIntensity;
extern time_t   now; 


void readLastStatus();
void writeLastStatus();
bool readFileById(const char* fType, uint8_t mId);
bool writeFileById(const char* fType, uint8_t mId, const char *msg);
void updateMessage(const char *field, const char *newValue);
void writeToLog(const char *logLine);

#endif // LITTLEFS_STUFF_H
