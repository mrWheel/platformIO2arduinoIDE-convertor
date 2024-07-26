/*
***************************************************************************
**  Program : WeerliveClass.cpp
**  Copyright (c) 2024 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
*/

#include "WeerliveClass.h"

const char *Weerlive::apiHost = "weerlive.nl";

const char *dayNames[] = {"Zondag", "Maandag", "Dinsdag", "Woensdag", "Donderdag", "Vrijdag", "Zaterdag"};

Weerlive::Weerlive(WiFiClient &thisClient) : thisClient(thisClient), apiUrl("") {}

/**
 * Initializes the Weerlive class by setting up the API URL with the provided key and city,
 * then starts the serial communication and configures filters.
 *
 * @param key The key for the API
 * @param city The city for which weather data is requested
 *
 * @throws None
 */
void Weerlive::setup(const char *key, const char *city)
{
  char tmp[100];
  Serial.begin(115200);
  while(!Serial)
  {
    delay(10);
  }
  Serial.flush();
  Serial.println("\nWeerlive setup");
  snprintf(tmp, sizeof(tmp), "/api/weerlive_api_v2.php?key=%s&locatie=%s", key, city);
  apiUrl = String(tmp);
  Serial.printf("setup(): apiUrl = [%s]\n", apiUrl.c_str());
  configureFilters();

} //  setup()

/**
 * Configures the filters for the Weerlive class.
 *
 * Sets up the filters for the 'liveweer' and 'wk_verw' objects to include specific
 * weather data.
 * The filters are used to determine which weather data to retrieve from the API.
 *
 * @throws None
 */
void Weerlive::configureFilters()
{
  filter["liveweer"][0]["plaats"]   = true;
  filter["liveweer"][0]["time"]     = false;
  filter["liveweer"][0]["temp"]     = true;   //-- actuele temperatuur in graden Celsius
  filter["liveweer"][0]["gtemp"]    = false;  //-- gevoeld temperatuur
  filter["liveweer"][0]["samenv"]   = true;   //-- samenvatting
  filter["liveweer"][0]["lv"]       = true;   //-- relatieve luchtvochtigheid
  filter["liveweer"][0]["windr"]    = true;   //-- windrichting
  filter["liveweer"][0]["windrgr"]  = false;  //-- windrichting in graden
  filter["liveweer"][0]["windms"]   = false;  //-- windkracht in m/s
  filter["liveweer"][0]["windbft"]  = true;   //-- windkracht in bft
  filter["liveweer"][0]["windknp"]  = false;  //-- windsnelheid in knoppen
  filter["liveweer"][0]["windkmh"]  = false;  //-- windsnelheid in km/h
  filter["liveweer"][0]["luchtd"]   = true;   //-- luchtdruk
  filter["liveweer"][0]["ldmmhg"]   = false;  //-- luchtdruk in mmHg
  filter["liveweer"][0]["dauwp"]    = true;   //-- dauwpunt
  filter["liveweer"][0]["zicht"]    = false;  //-- zicht in meters
  filter["liveweer"][0]["gr"]       = true;   //-- globale (zonne)straling in Watt/M2
  filter["liveweer"][0]["verw"]     = false;  //-- korte dagverwachting
  filter["liveweer"][0]["sup"]      = false;  //-- zon op
  filter["liveweer"][0]["sunder"]   = true;   //-- zon onder
  filter["liveweer"][0]["image"]    = true;   //-- afbeeldingsnaam
  filter["liveweer"][0]["alarm"]    = true;   //-- Geldt er een weerwaarschuwing? 1 = ja, 0 = nee
  filter["liveweer"][0]["lkop"]     = true;   //-- Weerwaarschuwing korte omschrijving
  filter["liveweer"][0]["ltekst"]   = false;  //-- Langere beschrijving van de waarschuwing
  filter["liveweer"][0]["wrschklr"] = false;  //-- KNMI kleurcode voor jouw regio
  filter["liveweer"][0]["wrsch_g"]  = false;  //-- Moment waarop de eerstkomende KNMI-waarschuwing geldt
  filter["liveweer"][0]["wrsch_gts"]= false;  //-- Timestamp van wrsch_g
  filter["liveweer"][0]["wrsch_gc"] = false;  //-- KNMI kleurcode voor de eerstkomende waarschuwing
  //---- week verwachting ---
  for(int i = 0; i < 4; i++)
  {
    filter["wk_verw"][i]["dag"]       = true;   //-- datum van deze dag
    filter["wk_verw"][i]["image"]     = true;   //-- afbeeldingsnaamafbeeldingsnaam
    filter["wk_verw"][i]["max_temp"]  = true;   //-- maximum temperatuur voor de dag
    filter["wk_verw"][i]["min_temp"]  = true;   //-- minimum temperatuur voor de dag
    filter["wk_verw"][i]["windbft"]   = false;   //--  windkracht in Beaufort
    filter["wk_verw"][i]["windkmh"]   = false;   //-- windkracht in km/h
    filter["wk_verw"][i]["windms"]    = false;   //-- windkracht in m/s
    filter["wk_verw"][i]["windknp"]   = false;   //-- windkracht in knopen
    filter["wk_verw"][i]["windrgr"]   = false;   //-- windrichting in graden
    filter["wk_verw"][i]["windr"]     = false;   //-- windrichting
    filter["wk_verw"][i]["neersl_perc_dag"] = true;   //-- kans op neerslag per dag
    filter["wk_verw"][i]["zond_perc_dag"]   = false;   //-- kans op zon per dag
  }

} //  configureFilters()

/**
 * Makes a request to the Weerlive API and processes the response.
 *
 * @return a pointer to a C-style string containing the processed weather data.
 *
 * @throws None.
 */
const char *Weerlive::request()
{
  int   weerliveStatus = 0;
  char  tmpMessage[30] = {};
  bool  gotData        = false;

  if (apiUrl.isEmpty())
  {
    weerliveText = "API URL not set";
    Serial.println(weerliveText);
    return weerliveText.c_str();
  }

  Serial.printf("\nWeerlive request [%s]\n", apiHost);
  Serial.printf("Free Heap: [%d] bytes\n", ESP.getFreeHeap());
#ifdef DEBUG
  Serial.println(apiUrl);
#endif  // DEBUG
  thisClient.setTimeout(2000);
  Serial.printf("\nNext: connect(%s, %d)\n\n", apiHost, 80);
  if (!thisClient.connect(apiHost, 80))
  {
    Serial.printf("connection to [%s] failed\n", apiHost);
    thisClient.flush();
    thisClient.stop();
    return "Error";
  }

  Serial.printf("connected to [%s]\n", apiHost);
  //-- This will send the request to the server
  Serial.print(String("GET ") + apiUrl + " HTTP/1.1\r\n" +
               "Host: " + apiHost + "\r\n" +
               "Connection: close\r\n\r\n");
  thisClient.print(String("GET ") + apiUrl + " HTTP/1.1\r\n" +
                   "Host: " + apiHost + "\r\n" +
                   "Connection: close\r\n\r\n");
  delay(10);

  while ((thisClient.connected() || thisClient.available()) && !gotData)
  {
    yield();
    while(thisClient.available() && !gotData)
    {
      //--- skip to find HTTP/1.1
      //--- then parse response code
      if (thisClient.find("HTTP/1.1"))
      {
        //-- parse status code
        weerliveStatus = thisClient.parseInt();
        snprintf(tmpMessage, sizeof(tmpMessage), "Weerlive: Statuscode: [%d] ", weerliveStatus);
        Serial.print(tmpMessage);
        if (weerliveStatus != 200)
        {
          Serial.println(" ERROR!");
          thisClient.flush();
          thisClient.stop();
          return tmpMessage;
        }
        Serial.println(" OK!");
      }
      else
      {
        Serial.println("Error reading weerLive.. -> bailout!");
        thisClient.flush();
        thisClient.stop();
        return "Error reading Weerlive -> bailout";
      }
      //--- skip headers
      if (thisClient.find("\r\n\r\n"))
      {
        //-- search for first occurence of a '{'
        thisClient.find("{");
        int charsRead = 0;
        //-- now read the rest of the response and put it in jsonResponse from pos 1 on
        charsRead = thisClient.readBytesUntil('\0',  jsonResponse+1, sizeof(jsonResponse)-1);
        //-- The first '{' is not in jsonResponse so put it at position 0
        jsonResponse[0] = '{';
#ifdef DEBUG
        Serial.printf("charsRead[%d]\n", charsRead);
#endif  // DEBUG
        jsonResponse[charsRead-1] = '\0';
        jsonResponse[charsRead]   = '\0';
#ifdef DEBUG
        Serial.printf("got weerdata [%d] bytes\n===================\n%s\n========================\n\n", strlen(jsonResponse), jsonResponse);

        Serial.printf("[0] is [%c], charsRead is [", jsonResponse[0]);
        Serial.print(jsonResponse[charsRead], HEX);
        Serial.println("]");
        Serial.printf("strlen(jsonResponse)[%d] >= sizeof(jsonResponse)[%d]\n", strlen(jsonResponse), sizeof(jsonResponse));
#endif  // DEBUG
        if (strlen(jsonResponse) >= (sizeof(jsonResponse)-2))
        {
          int p = charsRead;
          //-- if the jsonResponse is not complet look for the last '}'
          //-- from the last pos of jsonResponse towards the first pos
          //-- zond_perc_dag":64}],"uur_verw":[{"uur":"21-07-2024 11:00","timestamp":172
          //--                  ^<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
          for(p = (charsRead-4); p >= 0; p--)
          {
#ifdef DEBUG
            Serial.printf("Pos[%d] is [%c]\n", p, jsonResponse[p]);
#endif  // DEBUG
            if (jsonResponse[p] == '}') break;
          }
#ifdef DEBUG
          Serial.printf("Found '}' on pos[%d]\n", p);
          Serial.println((p-40));
          for(int x=p-40; jsonResponse[x] != 0; x++) Serial.print(jsonResponse[x]);
          Serial.println();
#endif  // DEBUG
          //-- now complete the jsonResponse by adding "]}"
          charsRead = p;
          jsonResponse[charsRead] = '}';
          jsonResponse[charsRead+1] = ']';
          jsonResponse[charsRead+2] = '}';
          jsonResponse[charsRead+3] = '\0';
          jsonResponse[charsRead+4] = '\0';
        }
        //-- we now have a complete correct JSON string in jsonResponse. Ready to parse it.
        gotData = true;
#ifdef DEBUG
        Serial.printf("got weerdata [%d] bytes\n===================\n%s\n========================\n\n", strlen(jsonResponse), jsonResponse);
#endif  // DEBUG
      }
    } // while available ..

  } // connected ..

  thisClient.flush();
  thisClient.stop();

  if (weerliveStatus == 200)
  {
#ifdef DEBUG
    Serial.printf("\njsonResponse ==[%d]===\n%s\n=============\n\n", strlen(jsonResponse), jsonResponse);
#endif  // DEBUG
    if (strlen(jsonResponse) == 0)
    {
      Serial.println(" ... Bailout!!\n");
      return weerliveText.c_str();
    }

    DeserializationError error = deserializeJson(doc, jsonResponse, DeserializationOption::Filter(filter));
    Serial.printf("After deserializeJson(): Free Heap: [%d] bytes\n", ESP.getFreeHeap());

    if (error)
    {
      weerliveText = "WeerliveClass: deserializeJson() failed: " + String(error.f_str());
      Serial.println(weerliveText);
      Serial.printf("Free Heap: [%d] bytes\n", ESP.getFreeHeap());
      return weerliveText.c_str();
    }

    weerliveText = "";

    Serial.println("\nProcessliveweer ......  \n");
    JsonArray liveweerArray = doc["liveweer"];
    Serial.printf("After liveweerArray: Free Heap [%d] bytes\n", ESP.getFreeHeap());
    alarmInd = -1;
    for (JsonObject weather : liveweerArray)
    {
      for (JsonPair kv : weather)
      {
#ifdef DEBUG
        Serial.print(kv.key().c_str());
        Serial.print(": ");
        Serial.println(kv.value().as<String>());
#endif  // DEBUG
        if (kv.key() == "plaats")
        {
          weerliveText += kv.value().as<String>();
        }
        else if (kv.key() == "time")
        {
          weerliveText += " " + kv.value().as<String>();
        }
        else if (kv.key() == "temp")
        {
          weerliveText += " " + kv.value().as<String>() + "°C";
        }
        else if (kv.key() == "gtemp")
        {
          weerliveText += " gevoelstemperatuur " + kv.value().as<String>() + "°C";
        }
        else if (kv.key() == "samenv")
        {
          weerliveText += " " + kv.value().as<String>();
        }
        else if (kv.key() == "lv")
        {
          weerliveText += " luchtvochtigheid " + kv.value().as<String>() + "%";
        }
        else if (kv.key() == "windr")
        {
          weerliveText += " windrichting " + kv.value().as<String>();
        }
        else if (kv.key() == "windrgr")
        {
          weerliveText += " windrichting " + kv.value().as<String>() + "°";
        }
        else if (kv.key() == "windms")
        {
          weerliveText += " " + kv.value().as<String>() + " m/s";
        }
        else if (kv.key() == "windbft")
        {
          weerliveText += " " + kv.value().as<String>() + " bft";
        }
        else if (kv.key() == "windknp")
        {
          weerliveText += " " + kv.value().as<String>() + " kts";
        }
        else if (kv.key() == "windkmh")
        {
          weerliveText += " " + kv.value().as<String>() + " km/h";
        }
        else if (kv.key() == "luchtd")
        {
          weerliveText += " luchtdruk " + kv.value().as<String>() + " hPa";
        }
        else if (kv.key() == "ldmmhg")
        {
          weerliveText += " luchtdruk " + kv.value().as<String>() + " mmHg";
        }
        else if (kv.key() == "dauwp")
        {
          weerliveText += " dauwpunt " + kv.value().as<String>() + "°C";
        }
        else if (kv.key() == "zicht")
        {
          weerliveText += " zicht " + kv.value().as<String>() +" m";
        }
        else if (kv.key() == "gr")
        {
          weerliveText += " globale (zonne)straling " + kv.value().as<String>() + " Watt/M2";
        }
        else if (kv.key() == "verw")
        {
          weerliveText += " dagverwachting: " + kv.value().as<String>();
        }
        else if (kv.key() == "sup")
        {
          weerliveText += " zon op " + kv.value().as<String>();
        }
        else if (kv.key() == "sunder")
        {
          weerliveText += " zon onder " + kv.value().as<String>();
        }
        else if (kv.key() == "image")
        {
          weerliveText += " " + kv.value().as<String>();
        }
        else if (kv.key() == "alarm")
        {
          alarmInd = kv.value().as<String>().toInt();
        }
        else if (kv.key() == "lkop" && (alarmInd == 1))
        {
          weerliveText += " waarschuwing: " + kv.value().as<String>();
        }
        else if (kv.key() == "ltekst" && (alarmInd == 1))
        {
          weerliveText += " waarschuwing: " + kv.value().as<String>();
        }
        else if (kv.key() == "wrschklr")
        {
          weerliveText += " KNMI kleurcode " + kv.value().as<String>();
        }
        else if (kv.key() == "wrsch_g"&& (alarmInd == 1))
        {
          weerliveText += " waarschuwing:" + kv.value().as<String>();
        }
        else if (kv.key() == "wrsch_gts" && (alarmInd == 1))
        {
          weerliveText += " Timestamp van waarschuwing " + kv.value().as<String>();
        }
        else if (kv.key() == "wrsch_gc" && (alarmInd == 1))
        {
          weerliveText += " kleurcode eerstkomende waarschuwing " + kv.value().as<String>();
        }
        else
        {
          //weerliveText += ", [" + String(kv.key().c_str()) + "] = " + kv.value().as<String>();
        }
      }
    }
    serializeJsonPretty(liveweerArray, Serial);

    Serial.println("\nProcess wk_verw ......  \n");
    // Process the wk_verw array
    JsonArray wk_verwArray = doc["wk_verw"];
    Serial.printf("After wk_verwArray: Free Heap [%d] bytes\n\n", ESP.getFreeHeap());
    for (JsonVariant v : wk_verwArray)
    {
      JsonObject obj = v.as<JsonObject>();
      for (JsonPair kv : obj)
      {
#ifdef DEBUG
        Serial.print(kv.key().c_str());
        Serial.print(": ");
        Serial.println(kv.value().as<String>());
#endif  // DEBUG

        if (kv.key() == "dag" && filter["wk_verw"][0]["dag"])
        {
          weerliveText += " | "+ String(dateToDayName(kv.value().as<String>().c_str()));
        }
        else if (kv.key() == "image" )
        {
          weerliveText += " " + kv.value().as<String>();
        }
        else if (kv.key() == "max_temp" )
        {
          weerliveText += " max " + kv.value().as<String>() + "°C";
        }
        else if (kv.key() == "min_temp" )
        {
          weerliveText += " min " + kv.value().as<String>() + "°C";
        }
        else if (kv.key() == "windbft")
        {
          weerliveText += " " + kv.value().as<String>() + " bft";
        }
        else if (kv.key() == "windkmh")
        {
          weerliveText += " " + kv.value().as<String>() + " km/h";
        }
        else if (kv.key() == "windms")
        {
          weerliveText += " " + kv.value().as<String>() + " m/s";
        }
        else if (kv.key() == "windknp")
        {
          weerliveText += " " + kv.value().as<String>() + " kts";
        }
        else if (kv.key() == "windrgr")
        {
          weerliveText += " windrichting " + kv.value().as<String>() + "°";
        }
        else if (kv.key() == "windr")
        {
          weerliveText += " windrichting " + kv.value().as<String>();
        }
        else if (kv.key() == "neersl_perc_dag")
        {
          weerliveText += " neerslag kans " + kv.value().as<String>() + "%";
        }
        else if (kv.key() == "zond_perc_dag")
        {
          weerliveText += " zon kans " + kv.value().as<String>() + "%";
        }
        else
        {
          //weerliveText += ", [" + String(kv.key().c_str()) + "] = " + kv.value().as<String>();
        }
      }
    }
#ifdef DEBUG
    Serial.printf("\nweerliveText is [%s]\n\n", weerliveText.c_str());
#endif  // DEBUG
  }
  else
  {
    weerliveText = "Error [" + String(weerliveStatus) + "] on HTTP request";
    Serial.println(weerliveText);
#ifdef DEBUG
    Serial.printf("[HTTP] GET... code: %d\n", weerliveStatus);
#endif  // DEBUG
    if (weerliveStatus == 429)
    {
      weerliveText = "WeerliveClass: status [429] Too many requests";
#ifdef DEBUG
      Serial.println(weerliveText);
#endif  // DEBUG
      return weerliveText.c_str();
    }
  }

  return weerliveText.c_str();


} //  request()

/**
 * Takes a date string in the format "day-month-year" and converts it to the
 * corresponding day of the week using Zeller’s Congruence
 *
 * @param date_str The date string in the format "day-month-year"
 *
 * @return A const char* representing the day of the week
 *
 * @throws None
 */
const char *Weerlive::dateToDayName(const char *date_str)
{
  int day, month, year;
  sscanf(date_str, "%d-%d-%d", &day, &month, &year);

  if (month < 3)
  {
    month += 12;
    year -= 1;
  }

  int K = year % 100;
  int J = year / 100;
  int f = day + 13 * (month + 1) / 5 + K + K / 4 + J / 4 + 5 * J;
  int dayOfWeek = f % 7;

  // Adjust dayOfWeek to fit our dayNames array (0 = Saturday, 1 = Sunday, ..., 6 = Friday)
  dayOfWeek = (dayOfWeek + 6) % 7;

  return dayNames[dayOfWeek];
}


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
