/*
***************************************************************************  
**  Program : networkStuff.h
**
**  Copyright (c) 2021 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
**      Usage:
**      
**      #define _HOSTNAME  thisProject      
**      
**      setup()
**      {
**        startTelnet();
**        //startMDNS(_HOSTNAME);
**        httpServer.on("/index",     <sendIndexPage>);
**        httpServer.on("/index.html",<sendIndexPage>);
**        httpServer.begin();
**      }
**      
**      loop()
**      {
**        handleWiFi();
**        //MDNS.update();
**        httpServer.handleClient();
**        .
**        .
**      }
*/

void displayMsg(String);

#include <ESP8266WiFi.h>        //ESP8266 Core WiFi Library         
#include <ESP8266WebServer.h>   // Version 1.0.0 - part of ESP8266 Core https://github.com/esp8266/Arduino
//#include <ESP8266mDNS.h>        // part of ESP8266 Core https://github.com/esp8266/Arduino

#include <WiFiUdp.h>            // part of ESP8266 Core https://github.com/esp8266/Arduino
#ifdef USE_UPDATE_SERVER
  //#include "ESP8266HTTPUpdateServer.h"
  #include <ModUpdateServer.h>   // https://github.com/mrWheel/ModUpdateServer
  #include "updateServerHtml.h"
#endif
#ifdef _WIFI_STA_ON
  #include <WiFiManager.h>       // version 0.15.0 - https://github.com/tzapu/WiFiManager
#endif
// included in main program: #include <TelnetStream.h>       // Version 0.0.1 - https://github.com/jandrassy/TelnetStream

//#include <WebSocketsServer.h> // https://github.com/Links2004/arduinoWebSockets

//WebSocketsServer webSocket = WebSocketsServer(81);

ESP8266WebServer        httpServer (80);
#ifdef USE_UPDATE_SERVER
  ESP8266HTTPUpdateServer httpUpdater(true);
#endif

//bool        LittleFSmounted; 
//bool        isConnected = false;

IPAddress local_IP(10,10,10,10);
IPAddress gateway(10,10,10,0);
IPAddress subnet(255,255,255,0);
const char* emptyPasswd = "";

#ifdef _WIFI_STA_ON
//gets called when WiFiManager enters configuration mode
//===========================================================================================
void configModeCallback (WiFiManager *myWiFiManager)
{
  DebugTln(F("Entered config mode\r"));
  DebugTln(WiFi.softAPIP().toString());
  //if you used auto generated SSID, print it
  yield();
  DebugTln(myWiFiManager->getConfigPortalSSID());

} // configModeCallback()
#endif

//===========================================================================================
void startWiFi(const char* hostname, int timeOut) 
{
  DebugTf("startWiFi(%s, %d)\r\n", hostname, timeOut);
  
  uint32_t lTime = millis();
  String thisAP = String(hostname) + "-" + WiFi.macAddress().substring(12);

  //WiFi.persistent(true);
  WiFi.mode(WIFI_OFF);
  delay(100);

#ifdef _WIFI_STA_ON
  DebugT("Setting WiFi.mode(WIFI_STA) ...");
  Debugln(WiFi.mode(WIFI_STA) ? "Ready" : "Failed!");

  WiFiManager manageWiFi;
  
  manageWiFi.setDebugOutput(true);

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  manageWiFi.setAPCallback(configModeCallback);

  //---sets timeout until configuration portal gets turned off
  //useful to make it all retry or go to sleep in seconds
  manageWiFi.setTimeout(240);  // 4 minuten

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "DSMRrecorder:<MAC>"
  //and goes into a blocking loop awaiting configuration
  if (!manageWiFi.autoConnect(thisAP.c_str()))
  //if (!manageWiFi.autoConnect(String(hostname).c_str()))
  {
    DebugTln(F("failed to connect and hit timeout"));
    TelnetStream.stop();
    //reset and try again, or maybe put it to deep sleep
    delay(3000);
    ESP.reset();
    delay(2000);
  }

  DebugTf("Connected with IP-address [%s]\r\n\r\n", WiFi.localIP().toString().c_str());

  #ifdef USE_UPDATE_SERVER
    httpUpdater.setup(&httpServer); // , update_path, update_username, update_password);
    httpUpdater.setIndexPage(UpdateServerIndex);
    httpUpdater.setSuccessPage(UpdateServerSuccess);
  #endif

#endif  // _WIFI_STA_ON

#ifdef _WIFI_AP_ON
  DebugT("Setting WiFi.mode(WIFI_AP) ...");
  Debugln(WiFi.mode(WIFI_AP) ? "Ready" : "Failed!");
  DebugFlush();
  delay(100);

  DebugT("Setting soft-AP configuration ... ");
  Debugln(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

  DebugTf("Setting soft-AP [%s] ... ", thisAP.c_str());
  Debugln(WiFi.softAP(thisAP, emptyPasswd) ? "Ready" : "Failed!");
  delay(300);

  yield();
  
  IPAddress IP = WiFi.softAPIP();
  DebugTf("Access Point IP address: [%s]\r\n", WiFi.softAPIP().toString().c_str());

  #ifdef USE_UPDATE_SERVER
    httpUpdater.setup(&httpServer); // , update_path, update_username, update_password);
    httpUpdater.setIndexPage(UpdateServerIndex);
    httpUpdater.setSuccessPage(UpdateServerSuccess);
  #endif

#endif  // _WIFI_AP_ON

  delay(1000);
  DebugTf("Setup WiFi and UpdateServer took [%d] seconds => OK!\r\n", ((millis() - lTime) / 1000)+1);
  
} // startWiFi()


//===========================================================================================
void startTelnet() 
{
  TelnetStream.begin();
  DebugTln(F("Telnet server started .."));
  TelnetStream.flush();
  while(TelnetStream.available()) 
  {
    TelnetStream.read();
    delay(5);
  }
  TelnetStream.flush();

} // startTelnet()

/****
//=======================================================================
void startMDNS(const char *Hostname) 
{
  DebugTf("[1] mDNS setup as [%s.local]\r\n", Hostname);
  if (MDNS.begin(Hostname))               // Start the mDNS responder for Hostname.local
  {
    DebugTf("[2] mDNS responder started as [%s.local]\r\n", Hostname);
  } 
  else 
  {
    DebugTln(F("[3] Error setting up MDNS responder!\r\n"));
  }
  MDNS.addService("http", "tcp", 80);
  
} // startMDNS()
****/
/***
//=====================================================================================
void doRedirect(String msg, int wait, const char* URL, bool reboot)
{
  String redirectHTML = 
  "<!DOCTYPE HTML><html lang='en-US'>"
  "<head>"
  "<meta charset='UTF-8'>"
  "<style type='text/css'>"
  "body {background-color: lightblue;}"
  "</style>"
  "<title>Redirect to Main Program</title>"
  "</head>"
  "<body><h1>"+String(_HOSTNAME)+"</h1>"
  "<h3>"+msg+"</h3>"
  "<br><div style='width: 500px; position: relative; font-size: 25px;'>"
  "  <div style='float: left;'>Redirect over &nbsp;</div>"
  "  <div style='float: left;' id='counter'>"+String(wait)+"</div>"
  "  <div style='float: left;'>&nbsp; seconden ...</div>"
  "  <div style='float: right;'>&nbsp;</div>"
  "</div>"
  "<!-- Note: don't tell people to `click` the link, just tell them that it is a link. -->"
  "<br><br><hr>If you are not redirected automatically, click this <a href='/'>Main Program</a>."
  "  <script>"
  "      setInterval(function() {"
  "          var div = document.querySelector('#counter');"
  "          var count = div.textContent * 1 - 1;"
  "          div.textContent = count;"
  "          if (count <= 0) {"
  "              window.location.replace('"+String(URL)+"'); "
  "          } "
  "      }, 1000); "
  "  </script> "
  "</body></html>\r\n";
  
  DebugTln(msg);
  httpServer.send(200, "text/html", redirectHTML);
  if (reboot) 
  {
    delay(5000);
    ESP.restart();
    delay(5000);
  }
  
} // doRedirect()


//=====================================================================================
void updateFirmware()
{
  DebugTln("Reguest /update ..");
#ifdef USE_UPDATE_SERVER
  DebugTln(F("Redirect to updateIndex .."));
  doRedirect("wait ... ", 1, "/updateIndex ", false);
#else
  doRedirect("UpdateServer not available", 10, "/", false);
#endif
      
} // updateFirmware()
**/

//=====================================================================================
void indexHtml()
{
  String index = "<html charset=\"UTF-8\">";
  index += "<style type='text/css'> body {background-color: lightblue;} </style>";
  index += "<body><h1>DSMRrecorder</h1><br/>";
  index += "<h3> &nbsp; &nbsp; " + String(_FW_VERSION) + "</h3>";
  index += "<p>Update Mode! please select a valid baudrate!</p>";
  index += "</br></body></html>";
  
  httpServer.send(200, "text/html", index);

} //  indexHtml()

/**
//=====================================================================================
void reBootESP()
{
  DebugTln(F("Redirect and ReBoot .."));
  doRedirect("Reboot P1Modbus Bridge ..", 30, "/", true);
      
} // reBootESP()
**/
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
