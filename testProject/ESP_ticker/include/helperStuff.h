#ifndef HELPERSTUFF_H
#define HELPERSTUFF_H

#include <Arduino.h>
#include <IPAddress.h>
#include <LittleFS.h>
#include "allDefines.h"
#include "littlefsStuff.h"

extern String noWords[MAX_NO_NO_WORDS+1];

// Function Prototypes
bool    compare(String x, String y);
bool    isValidIP(IPAddress ip);
bool    isNumericp(const char *timeStamp, int8_t len);
int8_t  splitString(String inStrng, char delimiter, String wOut[], uint8_t maxWords);
void    strConcat(char *dest, int maxLen, const char *src);
void    strConcat(char *dest, int maxLen, float v, int dec);
void    strConcat(char *dest, int maxLen, int v);
void    strToLower(char *src);
void    strCopy(char *dest, int maxLen, const char *src, int frm, int to);
void    strCopy(char *dest, int maxLen, const char *src);
void    strLTrim(char *dest, int maxLen, const char tChar);
void    strRTrim(char *dest, int maxLen, const char tChar);
void    strTrim(char *dest, int maxLen, const char tChar);
void    strRemoveAll(char *dest, int maxLen, const char tChar);
void    strTrimCntr(char *dest, int maxLen);
int     strIndex(const char *haystack, const char *needle, int start);
int     strIndex(const char *haystack, const char *needle);
int     stricmp(const char *a, const char *b);
char   *intToStr(int32_t v);
char   *floatToStr(float v, int dec);
float   formatFloat(float v, int dec);
float   strToFloat(const char *s, int dec);
void    parseJsonKey(const char *sIn, const char *key, char *val, int valLen);
uint8_t utf8Ascii(uint8_t ascii);
void    utf8Ascii(char* s);
void    getRevisionData(const char*);
void    splitNewsNoWords(const char *noNo);
bool    hasNoNoWord(const char *cIn);

#endif // HELPERSTUFF_H
