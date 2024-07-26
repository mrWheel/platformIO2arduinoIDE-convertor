#ifndef JSONSTUFF_H
#define JSONSTUFF_H

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "allDefines.h"


// Function prototypes
void sendStartJsonObj(const char *objName);
void sendEndJsonObj();
void sendNestedJsonObj(const char *cName, const char *cValue);
void sendNestedJsonObj(const char *cName, String sValue);
void sendNestedJsonObj(const char *cName, int32_t iValue);
void sendNestedJsonObj(const char *cName, uint32_t uValue);
void sendNestedJsonObj(const char *cName, float fValue);
void sendJsonSettingObj(const char *cName, float fValue, const char *fType, int minValue, int maxValue);
void sendJsonSettingObj(const char *cName, float fValue, const char *fType, int minValue, int maxValue, int decPlaces);
void sendJsonSettingObj(const char *cName, int iValue, const char *iType, int minValue, int maxValue);
void sendJsonSettingObj(const char *cName, const char *cValue, const char *sType, int maxLen);

// External global variables
extern ESP8266WebServer httpServer;

#endif // JSONSTUFF_H
