/*
***************************************************************************
**  Program  : helperStuff, part of ESP_ticker
**
**  Copyright (c) 2021 .. 2024 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
*/

#include "helperStuff.h"

//===========================================================================================
/**
 * Compares two strings lexicographically.
 *
 * @param x The first string to compare.
 * @param y The second string to compare.
 *
 * @return True if x is less than y, false otherwise.
 *
 * @throws None.
 */
bool compare(String x, String y)
{
  for (int i = 0; i < min(x.length(), y.length()); i++)
  {
    if (x[i] != y[i])
    {
      return (bool)(x[i] < y[i]);
    }
  }
  return x.length() < y.length();

} // compare()


//===========================================================================================
/**
 * Checks if the given IP address is valid.
 *
 * @param ip The IP address to check.
 *
 * @return True if the IP address is valid, false otherwise.
 *
 * @throws None.
 */
boolean isValidIP(IPAddress ip)
{
  /* Works as follows:
    *  example:
    *  127.0.0.1
    *   1 => 127||0||0||1 = 128>0 = true
    *   2 => !(false || false) = true
    *   3 => !(false || false || false || false ) = true
    *   4 => !(true && true && true && true) = false
    *   5 => !(false) = true
    *   true && true & true && false && true = false ==> correct, this is an invalid addres
    *
    *   0.0.0.0
    *   1 => 0||0||0||0 = 0>0 = false
    *   2 => !(true || true) = false
    *   3 => !(false || false || false || false) = true
    *   4 => !(true && true && true && tfalse) = true
    *   5 => !(false) = true
    *   false && false && true && true && true = false ==> correct, this is an invalid addres
    *
    *   192.168.0.1
    *   1 => 192||168||0||1 =233>0 = true
    *   2 => !(false || false) = true
    *   3 +> !(false || false || false || false) = true
    *   4 => !(false && false && true && true) = true
    *   5 => !(false) = true
    *   true & true & true && true && true = true ==> correct, this is a valid address
    *
    *   255.255.255.255
    *   1 => 255||255||255||255 =255>0 = true
    *   2 => !(false || false ) = true
    *   3 +> !(true || true || true || true) = false
    *   4 => !(false && false && false && false) = true
    *   5 => !(true) = false
    *   true && true && false && true && false = false ==> correct, this is an invalid address
    *
    *   0.123.12.1       => true && false && true && true && true = false  ==> correct, this is an invalid address
    *   10.0.0.0         => true && false && true && true && true = false  ==> correct, this is an invalid address
    *   10.255.0.1       => true && true && false && true && true = false  ==> correct, this is an invalid address
    *   150.150.255.150  => true && true && false && true && true = false  ==> correct, this is an invalid address
    *
    *   123.21.1.99      => true && true && true && true && true = true    ==> correct, this is annvalid address
    *   1.1.1.1          => true && true && true && true && true = true    ==> correct, this is annvalid address
    *
    *   Some references on valid ip addresses:
    *   - https://www.quora.com/How-do-you-identify-an-invalid-IP-address
    *
    */
  boolean _isValidIP = false;
  _isValidIP = ((ip[0] || ip[1] || ip[2] || ip[3])>0);                             // if any bits are set, then it is not 0.0.0.0
  _isValidIP &= !((ip[0]==0) || (ip[3]==0));                                       // if either the first or last is a 0, then it is invalid
  _isValidIP &= !((ip[0]==255) || (ip[1]==255) || (ip[2]==255) || (ip[3]==255)) ;  // if any of the octets is 255, then it is invalid
  _isValidIP &= !(ip[0]==127 && ip[1]==0 && ip[2]==0 && ip[3]==1);                 // if not 127.0.0.0 then it might be valid
  _isValidIP &= !(ip[0]>=224);                                                     // if ip[0] >=224 then reserved space

  Serial.printf( "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  if (_isValidIP)
    Serial.println(F(" = Valid IP"));
  else
    Serial.println(F(" = Invalid IP!"));

  return _isValidIP;

} //  isValidIP()


//===========================================================================================
/**
 * Checks if the input string contains only numeric characters.
 *
 * @param timeStamp The input string to be checked.
 * @param len The length of the input string.
 *
 * @return true if the input string contains only numeric characters, false otherwise.
 *
 * @throws None
 */
bool isNumericp(const char *timeStamp, int8_t len)
{
  for (int i=0; (i<len && i<12); i++)
  {
    if (timeStamp[i] < '0' || timeStamp[i] > '9')
    {
      return false;
    }
  }
  return true;

} // isNumericp()


//===========================================================================================
/**
 * Splits a string into multiple words based on a delimiter and stores them in an array.
 *
 * @param inStrng The input string to be split.
 * @param delimiter The character used as the delimiter to split the string.
 * @param wOut An array to store the split words.
 * @param maxWords The maximum number of words to be stored in the array.
 *
 * @return The number of words stored in the array.
 *
 * @throws None
 */
int8_t splitString(String inStrng, char delimiter, String wOut[], uint8_t maxWords)
{
  int16_t inxS = 0, inxE = 0, wordCount = 0;

  inStrng.trim();
  while(inxE < inStrng.length() && wordCount < maxWords)
  {
    inxE  = inStrng.indexOf(delimiter, inxS);         //finds location of first ,
    wOut[wordCount] = inStrng.substring(inxS, inxE);  //captures first data String
    wOut[wordCount].trim();
    //Serial.printf("[%d] => [%c] @[%d] found[%s]\r\n", wordCount, delimiter, inxE, wOut[wordCount].c_str());
    inxS = inxE;
    inxS++;
    wordCount++;
  }
  // zero rest of the words
  for(int i=wordCount; i< maxWords; i++)
  {
    wOut[wordCount][0] = 0;
  }
  // if not whole string processed place rest in last word
  if (inxS < inStrng.length())
  {
    wOut[maxWords-1] = inStrng.substring(inxS, inStrng.length());  // store rest of String
  }

  return wordCount;

} // splitString()


//===========================================================================================
/**
 * Concatenates a source string to a destination string if the resulting string length is within the specified maximum length.
 *
 * @param dest The destination character array where the source string will be concatenated.
 * @param maxLen The maximum length of the resulting string after concatenation.
 * @param src The source character array to be concatenated to the destination string.
 *
 * @throws None
 */
void strConcat(char *dest, int maxLen, const char *src)
{
  if (strlen(dest) + strlen(src) < maxLen)
  {
    strcat(dest, src);
  }
  else
  {
    Serial.printf("Combined string > %d chars\r\n", maxLen);
  }

} // strConcat()


//===========================================================================================
/**
 * Concatenates a formatted float value to a destination string if the resulting string length is within the specified maximum length.
 *
 * @param dest The destination character array where the formatted float value will be concatenated.
 * @param maxLen The maximum length of the resulting string after concatenation.
 * @param v The float value to be formatted and concatenated to the destination string.
 * @param dec The number of decimal places to include in the formatted float value.
 *
 * @throws None
 */
void strConcat(char *dest, int maxLen, float v, int dec)
{
  static char buff[25];
  if (dec == 0)       sprintf(buff, "%.0f", v);
  else if (dec == 1)  sprintf(buff, "%.1f", v);
  else if (dec == 2)  sprintf(buff, "%.2f", v);
  else if (dec == 3)  sprintf(buff, "%.3f", v);
  else if (dec == 4)  sprintf(buff, "%.4f", v);
  else if (dec == 5)  sprintf(buff, "%.5f", v);
  else                sprintf(buff, "%f",   v);

  if (strlen(dest) + strlen(buff) < maxLen)
  {
    strcat(dest, buff);
  }
  else
  {
    Serial.printf("Combined string > %d chars\r\n", maxLen);
  }

} // strConcat()


//===========================================================================================
/**
 * Concatenates an integer value to a character array if the resulting string length is within the specified maximum length.
 *
 * @param dest The destination character array where the integer value will be concatenated.
 * @param maxLen The maximum length of the resulting string after concatenation.
 * @param v The integer value to be converted to a string and concatenated.
 *
 * @throws None
 */
void strConcat(char *dest, int maxLen, int v)
{
  static char buff[25];
  sprintf(buff, "%d", v);

  if (strlen(dest) + strlen(buff) < maxLen)
  {
    strcat(dest, buff);
  }
  else
  {
    Serial.printf("Combined string > %d chars\r\n", maxLen);
  }

} // strConcat()


//===========================================================================================
/**
 * Converts the input string to lowercase.
 *
 * @param src The input string to be converted to lowercase.
 *
 * @return None
 *
 * @throws None
 */
void strToLower(char *src)
{
  for (int i = 0; i < strlen(src); i++)
  {
    if (src[i] == '\0') return;
    if (src[i] >= 'A' && src[i] <= 'Z')
      src[i] += 32;
  }
} // strToLower()

//===========================================================================================
// a 'save' string copy
/**
 * Copies a substring from the source string to the destination string.
 *
 * @param dest The destination string where the substring will be copied.
 * @param maxLen The maximum length of the destination string.
 * @param src The source string from which the substring will be copied.
 * @param frm The starting index of the substring in the source string.
 * @param to The ending index of the substring in the source string.
 *
 * @throws None
 */
void strCopy(char *dest, int maxLen, const char *src, int frm, int to)
{
  int d=0;
  //Serial.printf("dest[%s], src[%s] max[%d], frm[%d], to[%d] =>\r\n", dest, src, maxLen, frm, to);
  dest[0] = '\0';
  for (int i=0; i<=frm; i++)
  {
    if (src[i] == 0) return;
  }
  for (int i=frm; (src[i] != 0  && i<=to && d<maxLen); i++)
  {
    dest[d++] = src[i];
  }
  dest[d] = '\0';

} // strCopy()

//===========================================================================================
// a 'save' version of strncpy() that does not put a '\0' at
// the end of dest if src >= maxLen!
void strCopy(char *dest, int maxLen, const char *src)
{
  dest[0] = '\0';
  strcat(dest, src);

} // strCopy()

//===========================================================================================
// 'tttABCtDEtFGHttt' => 'ABCtDEtFGHttt'
void strLTrim(char *dest, int maxLen, const char tChar )
{
  char tmp[maxLen];
  int  tPos = 0;
  bool done = false;

  tmp[0] = '\0';

  for (int dPos=0; (dPos<maxLen && dest[dPos] != '\0'); dPos++)
  {
    if (dest[dPos] != tChar || done)
    {
      tmp[tPos++] = dest[dPos];
      done = true;
    }
  }
  tmp[tPos] = '\0';
  strCopy(dest, maxLen, tmp);

} // strLTrim()

//===========================================================================================
// 'tttABCtDEtFGHttt' => 'tttABCtDEtFGH'
void strRTrim(char *dest, int maxLen, const char tChar )
{
  char tmp[maxLen];
  int dPos, tPos, dMax;
  bool done = false;

  for(dMax=0; dMax<maxLen; dMax++)
  {
    tmp[dMax] = '\0';
  }

  dMax = strlen(dest)-1;
  for (dPos=dMax; (dPos>=0 && !done); dPos--)
  {
    if (dest[dPos] == tChar)
    {
      tPos = dPos;
      tmp[tPos] = '\0';
    }
    else done = true;
  }
  dPos++;
  for(dMax = 0; dMax <= dPos; dMax++)
  {
    tmp[dMax] = dest[dMax];
  }
  tmp[dMax+1] = '\0';
  strCopy(dest, maxLen, tmp);

} // strRTrim()

//===========================================================================================
// 'tttABCtDEtFGHttt' => 'ABCtDEtFGH'
void strTrim(char *dest, int maxLen, const char tChar )
{
  char sTmp[maxLen];

  strCopy(sTmp, maxLen, dest);
  strLTrim(sTmp, maxLen, tChar);
  strRTrim(sTmp, maxLen, tChar);
  strCopy(dest, maxLen, sTmp);

} // strTrim()

//===========================================================================================
/**
 * Removes all occurrences of a specified character from a given string and stores the result in the destination string.
 *
 * @param dest The destination string where the modified string will be stored.
 * @param maxLen The maximum length of the destination string.
 * @param tChar The character to be removed from the string.
 *
 * @throws None
 */
void strRemoveAll(char *dest, int maxLen, const char tChar)
{
  char tmp[maxLen];
  int  tPos = 0;
  tmp[0] = '\0';
  for (int dPos=0; (dPos<maxLen && dest[dPos] != '\0'); dPos++)
  {
    if (dest[dPos] != tChar)
    {
      tmp[tPos++] = dest[dPos];
    }
  }
  tmp[tPos] = '\0';
  strCopy(dest, maxLen, tmp);

} // strRemoveAll()


//===========================================================================================
/**
 * Trims non-printable characters from the input string and stores the result in the destination string.
 *
 * @param dest The destination string where the modified string will be stored.
 * @param maxLen The maximum length of the destination string.
 *
 * @return None
 *
 * @throws None
 */
void strTrimCntr(char *dest, int maxLen)
{
  char tmp[maxLen];
  int tPos = 0;
  tmp[0] = '\0';
  for (int dPos=0; (dPos<maxLen && dest[dPos] != '\0'); dPos++)
  {
    if (dest[dPos] >= ' ' && dest[dPos] <= '~')  // space = 32, '~' = 127
    {
      tmp[tPos++] = dest[dPos];
    }
  }
  tmp[tPos] = '\0';
  strCopy(dest, maxLen, tmp);

} // strTrimCntr()

//===========================================================================================
/**
 * Finds the first occurrence of a substring in a string starting from a specific index.
 *
 * @param haystack The string to search in.
 * @param needle The substring to search for.
 * @param start The index from where to start the search.
 *
 * @return The index of the first occurrence of the substring in the string, or -1 if not found.
 *
 * @throws None.
 */
int strIndex(const char *haystack, const char *needle, int start)
{
  // strindex(hay, needle) ????
  char *p = strstr(haystack+start, needle);
  if (p)
  {
    //Serial.printf("found [%s] at position [%d]\r\n", needle, (p - haystack));
    return (p - haystack);
  }
  return -1;

} // strIndex()

//===========================================================================================
/**
 * Finds the first occurrence of a substring in a string starting from the beginning.
 *
 * @param haystack The string to search in.
 * @param needle The substring to search for.
 *
 * @return The index of the first occurrence of the substring in the string, or -1 if not found.
 *
 * @throws None.
 */
int strIndex(const char *haystack, const char *needle)
{
  return strIndex(haystack, needle, 0);

} // strIndex()

//===========================================================================================
/**
 * Compare two strings ignoring case.
 *
 * @param a The first string to compare.
 * @param b The second string to compare.
 *
 * @return The difference in ASCII value of the first differing character in the two strings.
 *
 * @throws None.
 */
int stricmp(const char *a, const char *b)
{
  for (;; a++, b++)
  {
    int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
    if (d != 0 || !*a)
      return d;
  }

} // stricmp()

//===========================================================================================
/**
 * Converts an integer value to a string representation.
 *
 * @param v The integer value to be converted.
 *
 * @return A pointer to a character array containing the string representation of the integer.
 *
 * @throws None.
 */
char *intToStr(int32_t v)
{
  static char buff[25];
  sprintf(buff, "%d", v);
  return buff;

} // intToStr()

//===========================================================================================
/**
 * Converts a floating-point number to a string representation with a specified number of decimal places.
 *
 * @param v The floating-point number to be converted.
 * @param dec The number of decimal places in the output string.
 *
 * @return A pointer to a character array containing the string representation of the floating-point number.
 *
 * @throws None.
 */
char *floatToStr(float v, int dec)
{
  static char buff[25];
  if (dec == 0)       sprintf(buff, "%.0f", v);
  else if (dec == 1)  sprintf(buff, "%.1f", v);
  else if (dec == 2)  sprintf(buff, "%.2f", v);
  else if (dec == 3)  sprintf(buff, "%.3f", v);
  else if (dec == 4)  sprintf(buff, "%.4f", v);
  else if (dec == 5)  sprintf(buff, "%.5f", v);
  else                sprintf(buff, "%f",   v);
  return buff;

} // floattToStr()

//===========================================================================================
/**
 * Converts a floating-point number to a string representation with a specified number of decimal places.
 *
 * @param v The floating-point number to be converted.
 * @param dec The number of decimal places in the output string.
 *
 * @return The floating-point number as a string representation.
 *
 * @throws None.
 */
float formatFloat(float v, int dec)
{
  return (String(v, dec).toFloat());

} //  formatFloat()

//===========================================================================================
/**
 * Converts a string to a floating-point number with a specified number of decimal places.
 *
 * @param s The input string to convert.
 * @param dec The number of decimal places to round to.
 *
 * @return The floating-point number converted from the string.
 *
 * @throws None.
 */
float strToFloat(const char *s, int dec)
{
  float r =  0.0;
  int   p =  0;
  int   d = -1;

  r = strtof(s, NULL);
  p = (int)(r*pow(10, dec));
  r = p / pow(10, dec);
  //Serial.printf("[%s][%d] => p[%d] -> r[%f]\r\n", s, dec, p, r);
  return r;

} //  strToFloat()

//===========================================================================================
/**
 * Parses a JSON key-value pair and extracts the value associated with the given key.
 *
 * @param sIn The input JSON string.
 * @param key The key to search for in the JSON string.
 * @param val The output buffer to store the extracted value.
 * @param valLen The maximum length of the output buffer.
 *
 * @throws None.
 */
void parseJsonKey(const char *sIn, const char *key, char *val, int valLen)
{
  // json key-value pair looks like:
  //      "samenv": "Zwaar bewolkt",
  // or   "samenv": "Zwaar bewolkt"}
  int keyStart   = strIndex(sIn, key);
  int sepStart   = strIndex(sIn, ",", keyStart);
  if (sepStart == -1)
  {
    sepStart   = strIndex(sIn, "}", keyStart);
  }
  strCopy(val, valLen, sIn, keyStart+strlen(key), sepStart);
  strRemoveAll(val, valLen, ':');
  strRemoveAll(val, valLen, ',');
  strRemoveAll(val, valLen, '}');
  strRemoveAll(val, valLen, '"');
  strTrim(val, valLen, ' ');

} // parseJsonKey()

//===========================================================================================
/**
 *
 * @param ascii the UTF8 character to convert
 *
 * @return the Extended ASCII character
 *
 * @throws None
 */
uint8_t utf8Ascii(uint8_t ascii)
// Convert a single Character from UTF8 to Extended ASCII according to ISO 8859-1,
// also called ISO Latin-1. Codes 128-159 contain the Microsoft Windows Latin-1
// extended characters:
// - codes 0..127 are identical in ASCII and UTF-8
// - codes 160..191 in ISO-8859-1 and Windows-1252 are two-byte characters in UTF-8
//                 + 0xC2 then second byte identical to the extended ASCII code.
// - codes 192..255 in ISO-8859-1 and Windows-1252 are two-byte characters in UTF-8
//                 + 0xC3 then second byte differs only in the first two bits to extended ASCII code.
// - codes 128..159 in Windows-1252 are different, but usually only the €-symbol will be needed from this range.
//                 + The euro symbol is 0x80 in Windows-1252, 0xa4 in ISO-8859-15, and 0xe2 0x82 0xac in UTF-8.
//
// Modified from original code at http://playground.arduino.cc/Main/Utf8ascii
// Extended ASCII encoding should match the characters at http://www.ascii-code.com/
//
// Return "0" if a byte has to be ignored.
{
  static uint8_t cPrev;
  uint8_t c = '\0';

  if (ascii < 0x7f)   // Standard ASCII-set 0..0x7F, no conversion
  {
    cPrev = '\0';
    c = ascii;
  }
  else
  {
    switch (cPrev)  // Conversion depending on preceding UTF8-character
    {
      case 0xC2:
        c = ascii;
        break;
      case 0xC3:
        c = ascii | 0xC0;
        break;
      case 0x82:
        if (ascii==0xAC) c = 0x80; // Euro symbol special case
    }
    cPrev = ascii;   // save last char
  }
  //Serial.printf("\nConverted 0x%02x", ascii);
  //Serial.printf(" to 0x%02x", c);

  return(c);

} // utf8Ascii(uint8_t)

//===========================================================================================
void utf8Ascii(char *s)
// In place conversion UTF-8 string to Extended ASCII
// The extended ASCII string is always shorter.
{
  uint8_t c;
  char *cp = s;

  //Serial.printf("\nConverting: %c", s);

  while (*s != '\0')
  {
    c = utf8Ascii(*s++);
    if (c != '\0')
      *cp++ = c;
  }
  *cp = '\0';   // terminate the new string

} // utf8Ascii(char)


void getRevisionData(const char *fwVersion)
{
  if (!LittleFS.exists("/newsFiles/LCL-000"))
  {
    char LCL000[100];
    sprintf(LCL000, "ESP_ticker %s (c) by Willem Aandewiel", fwVersion);
    writeFileById("LCL", 0, LCL000);
  }

} //  getRevisionData()

//---------------------------------------------------------------------
void splitNewsNoWords(const char *noNo)
{
  Serial.println(noNo);
  int8_t wc = splitString(String(noNo), ' ', noWords, MAX_NO_NO_WORDS);
  for(int8_t i=0; i<wc; i++)
  {
    noWords[i].trim();
    if (noWords[i].length() > 1)
    {
      noWords[i].toLowerCase();
      Serial.printf("NoNoWord[%d] [%s]\r\n", i, noWords[i].c_str());
    }
  }

} // splitNewsNoWords()

//---------------------------------------------------------------------
bool hasNoNoWord(const char *cIn)
{
  for(int8_t i=0; i<MAX_NO_NO_WORDS; i++)
  {
    String sIn = String(cIn);
    sIn.toLowerCase();
    int idx = sIn.indexOf(noWords[i]);
    if ((idx > -1) && (noWords[i].length() > 1))  // yes! it's in there somewhere
    {
      Serial.printf("found [%s]\r\n", noWords[i].c_str());
      return true;
    }
  }
  //Serial.println("no NoNo words found!");
  return false;

} // hasNoNoWord()


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