/*
***************************************************************************
**  Program  : littlefsStuff, part of ESP_ticker
**
**  Copyright (c) 2021 .. 2024 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
*/

#include "littlefsStuff.h"

//====================================================================
/**
 * Reads the last status from the "/sysStatus.csv" file and prints
 * the values.
 *
 * @throws None
 */
void readLastStatus()
{
  char buffer[50] = "";
  char dummy[50] = "";

  File _file = LittleFS.open("/sysStatus.csv", "r");
  if (!_file)
  {
    Serial.println("read(): No /sysStatus.csv found ..");
  }
  if(_file.available())
  {
    int l = _file.readBytesUntil('\n', buffer, sizeof(buffer));
    buffer[l] = 0;
    Serial.printf("read lastUpdate[%s]\r\n", buffer);
    sscanf(buffer, "%[^;]; %[^;]; %u; %[^;]", cDate, cTime, &nrReboots, dummy);
    Serial.printf("values timestamp[%s %s], nrReboots[%u], dummy[%s]\r\n"
                  , cDate
                  , cTime
                  , nrReboots
                  , dummy);
    yield();
  }
  _file.close();

}  // readLastStatus()


//====================================================================
/**
 * Writes the last status information to the "/sysStatus.csv" file
 * including date, time, number of reboots, and meta data.
 *
 * @param None
 *
 * @return None
 *
 * @throws None
 */
void writeLastStatus()
{
  if (ESP.getFreeHeap() < 8500) // to prevent firmware from crashing!
  {
    Serial.printf("Bailout due to low heap (%d bytes)\r\n", ESP.getFreeHeap());
    return;
  }
  char buffer[50] = "";
  snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d; %02d:%02d:%02d; %010u; %s;\n"
           , localtime(&now)->tm_year+1900, localtime(&now)->tm_mon+1, localtime(&now)->tm_mday
           , localtime(&now)->tm_hour, localtime(&now)->tm_min, localtime(&now)->tm_sec
           , nrReboots
           , "meta data");
  Serial.printf("writeLastStatus() => %s\r\n", buffer);

  File _file = LittleFS.open("/sysStatus.csv", "w");
  if (!_file)
  {
    Serial.println("write(): No /sysStatus.csv found ..");
  }
  _file.print(buffer);
  _file.flush();
  _file.close();

} // writeLastStatus()


//------------------------------------------------------------------------
/**
 * Reads a file from LittleFS by its type and ID.
 *
 * @param fType The type of the file.
 * @param mId The ID of the file.
 *
 * @return True if the file is read successfully, false otherwise.
 *
 * @throws None
 */
bool readFileById(const char *fType, uint8_t mId)
{
  String percChar   = "%%";
  String backSlash  = "\\";
  String rTmp;
  char fName[50] = "";

  sprintf(fName, "/newsFiles/%s-%03d", fType, mId);

  Serial.printf("read [%s] ", fName);

  if (!LittleFS.exists(fName))
  {
    Serial.println("Does not exist!");
    return false;
  }

  File f = LittleFS.open(fName, "r");

  while(f.available())
  {
    rTmp = f.readStringUntil('\n');
    //Serial.printf("rTmp(in)  [%s]\r\n", rTmp.c_str());
    rTmp.replace("\r", "");
  }
  f.close();

  rTmp.replace("@1@", ":");
  rTmp.replace("@2@", "{");
  rTmp.replace("@3@", "}");
  rTmp.replace("@4@", ",");
  rTmp.replace("@5@", backSlash);
  rTmp.replace("@6@", percChar);
  //Serial.printf("rTmp(out) [%s]\r\n", rTmp.c_str());

  snprintf(fileMessage, LCL_SIZE, rTmp.c_str());
  if (strlen(fileMessage) == 0)
  {
    Serial.println("file is zero bytes long");
    return false;
  }
  Serial.printf("OK! \r\n\t[%s]\r\n", fileMessage);

  if (mId == 0)
  {
    LittleFS.remove("/newsFiles/LCL-000");
    Serial.println("Remove LCL-000 ..");
  }

  return true;

} // readFileById()

//------------------------------------------------------------------------
/**
 * Writes a message to a file specified by file type and message ID.
 *
 * @param fType The type of the file
 * @param mId The ID of the message
 * @param msg The message to write to the file
 *
 * @return true if the message is successfully written to the file, false otherwise
 *
 * @throws None
 */
bool writeFileById(const char *fType, uint8_t mId, const char *msg)
{
  String rTmp;
  char fmsg[LCL_SIZE] = "";
  char fName[50] = "";
  sprintf(fName, "/newsFiles/%s-%03d", fType, mId);

  Serial.printf("write [%s]-> [%s]\r\n", fName, msg);

  if (strlen(msg) < 3)
  {
    LittleFS.remove(fName);
    Serial.println("Empty message, file removed!");
    return true;
  }

  Serial.println("LittleFS.open()...");
  File file = LittleFS.open(fName, "w");
  if (!file)
  {
    Serial.printf("open(%s, 'w') FAILED!!! --> Bailout\r\n", fName);
    return false;
  }
  yield();

  Serial.println(F("Start writing data .. \r"));
  Serial.flush();
  snprintf(fmsg, LCL_SIZE, "%s", msg);
  Serial.println(fmsg);
  file.println(fmsg);
  file.close();

  Serial.println("Exit writeFileById()!");
  return true;

} // writeFileById()


//=======================================================================
/**
 * Updates a message in the LittleFS filesystem.
 *
 * @param field The field of the message to update.
 * @param newValue The new value of the message.
 *
 * @throws None
 */
void updateMessage(const char *field, const char *newValue)
{
  int8_t msgId = String(field).toInt();

  Serial.printf("-> field[%s], newValue[%s]\r\n", field, newValue);

  if (msgId < 0 || msgId > settingLocalMaxMsg)
  {
    Serial.printf("msgId[%d] is out of scope! Bailing out!\r\n", msgId);
    return;
  }

  writeFileById("LCL", msgId, newValue);

} // updateMessage()


//====================================================================
/**
 * Writes a log line to a file in the LittleFS filesystem.
 *
 * @param logLine The line to be written to the log.
 *
 * @return None
 *
 * @throws None
 */
void writeToLog(const char *logLine)
{
  if (ESP.getFreeHeap() < 8500) // to prevent firmware from crashing!
  {
    Serial.printf("Bailout due to low heap (%d bytes)\r\n", ESP.getFreeHeap());
    return;
  }
  char buffer[150] = "";
  snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d; %02d:%02d:%02d; %s;\n"
           , localtime(&now)->tm_year+1900, localtime(&now)->tm_mon+1, localtime(&now)->tm_mday
           , localtime(&now)->tm_hour, localtime(&now)->tm_min, localtime(&now)->tm_sec
           , logLine);
  Serial.printf("writeToLogs() => %s\r\n", buffer);
  File _file = LittleFS.open("/sysLog.csv", "a");
  if (!_file)
  {
    Serial.println("write(): No /sysLog.csv found ..");
  }
  _file.print(buffer);
  _file.flush();
  _file.close();

} // writeLastStatus()



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