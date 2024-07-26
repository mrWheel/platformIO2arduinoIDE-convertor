#ifndef SENDINDEXPAGE_H
#define SENDINDEXPAGE_H

#include <ESP8266WebServer.h>
#include "networkStuff.h"

extern const char index_html[];

void sendIndexPage();
void handleRoot(ESP8266WebServer &server);

#endif
