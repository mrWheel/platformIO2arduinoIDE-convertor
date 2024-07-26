/*
***************************************************************************
**  Program  : jsonStuff, part of ESP_ticker
**
**  Copyright (c) 2021 .. 2024 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
*/

#include "jsonStuff.h"

static char objSprtr[10] = "";

//=======================================================================
/**
 * Sends the start of a JSON object with the specified object name.
 *
 * @param objName The name of the JSON object.
 *
 * @return void
 *
 * @throws None
 */
void sendStartJsonObj(const char *objName)
{
  char sBuff[50] = "";
  objSprtr[0]    = '\0';

  snprintf(sBuff, sizeof(sBuff), "{\"%s\":[\r\n", objName);
  httpServer.sendHeader("Access-Control-Allow-Origin", "*");
  httpServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
  httpServer.send(200, "application/json", sBuff);

} // sendStartJsonObj()


//=======================================================================
/**
 * Sends the end of a JSON object by sending the closing square bracket and curly brace.
 *
 * @throws None
 */
void sendEndJsonObj()
{
  httpServer.sendContent("\r\n]}\r\n");

} // sendEndJsonObj()


//=======================================================================
/**
 * Sends a nested JSON object with the given name and value.
 *
 * @param cName The name of the JSON object.
 * @param cValue The value of the JSON object.
 *
 * @return void
 *
 * @throws None
 */
void sendNestedJsonObj(const char *cName, const char *cValue)
{
  char jsonBuff[JSON_BUFF_MAX] = "";

  snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": \"%s\"}"
           , objSprtr, cName, cValue);

  httpServer.sendContent(jsonBuff);
  sprintf(objSprtr, ",\r\n");

} // sendNestedJsonObj(*char, *char)


//=======================================================================
/**
 * Sends a nested JSON object with a string value.
 *
 * @param cName The name of the JSON object.
 * @param sValue The string value of the JSON object.
 *
 * @throws None
 */
void sendNestedJsonObj(const char *cName, String sValue)
{
  char jsonBuff[JSON_BUFF_MAX] = "";

  if (sValue.length() > (JSON_BUFF_MAX - 65) )
  {
    Serial.printf("[2] sValue.length() [%d]\r\n", sValue.length());
  }

  snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": \"%s\"}"
           , objSprtr, cName, sValue.c_str());

  httpServer.sendContent(jsonBuff);
  sprintf(objSprtr, ",\r\n");

} // sendNestedJsonObj(*char, String)


//=======================================================================
/**
 * Sends a nested JSON object with the given name and integer value.
 *
 * @param cName The name of the JSON object.
 * @param iValue The integer value of the JSON object.
 *
 * @throws None.
 */
void sendNestedJsonObj(const char *cName, int32_t iValue)
{
  char jsonBuff[LCL_SIZE+25] = "";

  snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %d}"
           , objSprtr, cName, iValue);

  httpServer.sendContent(jsonBuff);
  sprintf(objSprtr, ",\r\n");

} // sendNestedJsonObj(*char, int)

//=======================================================================
/**
 * Sends a JSON object with the name and unsigned integer value provided.
 *
 * @param cName The name of the JSON object.
 * @param uValue The unsigned integer value of the JSON object.
 *
 * @return None.
 *
 * @throws None.
 */
void sendNestedJsonObj(const char *cName, uint32_t uValue)
{
  char jsonBuff[LCL_SIZE+25] = "";

  snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %u }"
           , objSprtr, cName, uValue);

  httpServer.sendContent(jsonBuff);
  sprintf(objSprtr, ",\r\n");

} // sendNestedJsonObj(*char, uint)


//=======================================================================
/**
 * Sends a JSON object with a name and a floating-point value to the HTTP server.
 *
 * @param cName The name of the JSON object.
 * @param fValue The floating-point value of the JSON object.
 *
 * @throws None
 */
void sendNestedJsonObj(const char *cName, float fValue)
{
  char jsonBuff[LCL_SIZE+25] = "";

  snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %.3f }"
           , objSprtr, cName, fValue);

  httpServer.sendContent(jsonBuff);
  sprintf(objSprtr, ",\r\n");

} // sendNestedJsonObj(*char, float)


//=======================================================================
// ************ function to build Json Settings string ******************
//=======================================================================
/**
 * Sends a JSON setting object with the given parameters.
 *
 * @param cName The name of the setting.
 * @param fValue The value of the setting.
 * @param fType The type of the setting.
 * @param minValue The minimum value of the setting.
 * @param maxValue The maximum value of the setting.
 *
 * @throws None.
 */
void sendJsonSettingObj(const char *cName, float fValue, const char *fType, int minValue, int maxValue)
{
  char jsonBuff[LCL_SIZE+25] = "";

  snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %.3f, \"type\": \"%s\", \"min\": %d, \"max\": %d}"
           , objSprtr, cName, fValue, fType, minValue, maxValue);

  httpServer.sendContent(jsonBuff);
  sprintf(objSprtr, ",\r\n");

} // sendJsonSettingObj(*char, float, *char, int, int)


//=======================================================================
/**
 * Sends a JSON setting object with the given parameters and a specific number of decimal places.
 *
 * @param cName The name of the setting.
 * @param fValue The value of the setting.
 * @param fType The type of the setting.
 * @param minValue The minimum value of the setting.
 * @param maxValue The maximum value of the setting.
 * @param decPlaces The number of decimal places for the value.
 *
 * @return None.
 *
 * @throws None.
 */
void sendJsonSettingObj(const char *cName, float fValue, const char *fType, int minValue, int maxValue, int decPlaces)
{
  char jsonBuff[LCL_SIZE+25] = "";

  switch(decPlaces)
  {
    case 0:
      snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %.0f, \"type\": \"%s\", \"min\": %d, \"max\": %d}"
               , objSprtr, cName, fValue, fType, minValue, maxValue);
      break;
    case 2:
      snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %.2f, \"type\": \"%s\", \"min\": %d, \"max\": %d}"
               , objSprtr, cName, fValue, fType, minValue, maxValue);
      break;
    case 5:
      snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %.5f, \"type\": \"%s\", \"min\": %d, \"max\": %d}"
               , objSprtr, cName, fValue, fType, minValue, maxValue);
      break;
    default:
      snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %f, \"type\": \"%s\", \"min\": %d, \"max\": %d}"
               , objSprtr, cName, fValue, fType, minValue, maxValue);

  }

  httpServer.sendContent(jsonBuff);
  sprintf(objSprtr, ",\r\n");

} // sendJsonSettingObj(*char, float, *char, int, int, int)


//=======================================================================
/**
 * Sends a JSON setting object with the given parameters.
 *
 * @param cName The name of the setting.
 * @param iValue The integer value of the setting.
 * @param iType The type of the setting.
 * @param minValue The minimum value of the setting.
 * @param maxValue The maximum value of the setting.
 *
 * @throws None.
 */
void sendJsonSettingObj(const char *cName, int iValue, const char *iType, int minValue, int maxValue)
{
  char jsonBuff[LCL_SIZE+25] = "";

  snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\": %d, \"type\": \"%s\", \"min\": %d, \"max\": %d}"
           , objSprtr, cName, iValue, iType, minValue, maxValue);

  httpServer.sendContent(jsonBuff);
  sprintf(objSprtr, ",\r\n");

} // sendJsonSettingObj(*char, int, *char, int, int)


//=======================================================================
/**
 * Sends a JSON setting object with the given parameters.
 *
 * @param cName The name of the setting.
 * @param cValue The value of the setting.
 * @param sType The type of the setting.
 * @param maxLen The maximum length of the setting value.
 *
 * @return None.
 *
 * @throws None.
 */
void sendJsonSettingObj(const char *cName, const char *cValue, const char *sType, int maxLen)
{
  char jsonBuff[LCL_SIZE+25] = "";

  snprintf(jsonBuff, sizeof(jsonBuff), "%s{\"name\": \"%s\", \"value\":\"%s\", \"type\": \"%s\", \"maxlen\": %d}"
           , objSprtr, cName, cValue, sType, maxLen);

  httpServer.sendContent(jsonBuff);
  sprintf(objSprtr, ",\r\n");

} // sendJsonSettingObj(*char, *char, *char, int, int)


/*
****************************************************************************
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