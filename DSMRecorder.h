/*
***************************************************************************  
**  Program : DSMRecorder.h
**
**  Copyright (c) 2021 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/
#ifndef DSMR_RECORDER_H
#define DSMR_RECORDER_H


#if defined(_WIFI_AP_ON) && defined(_WIFI_STA_ON)
#error STATION and Access Point can not both be Active
#endif
#if defined(_WIFI_AP_ON)
  #warning Compiled with AP always On
#elif defined(_WIFI_STA_ON)
  #warning Compiled with STA always On
#else
  #warning not compiled with AP or STA always On
#endif

#define USE_UPDATE_SERVER

#define _SETTINGS_FILE    "/DSMRecorder.ini"

#ifndef BUILTIN_LED
  #define BUILTIN_LED     2
#endif
#define _FLASH_BUTTON     0
#define _SHOW_BTN_TIME    2000
#define _DTR_ENABLE       12
#define _MAX_LOG_FILES    10

void menu(bool);  //-- prototype (see menuStuff)

#include <TelnetStream.h>
#include "Debug.h"
//-- version 20.05.2015 - https://github.com/Links2004/arduinoWebSockets
#include <WebSocketsServer.h>
#include "index_html.h"
#include "networkStuff.h"
//#include <dsmr2.h> 
//#include <dsmr.h> 
#include <LittleFS.h>
#define FSYS            LittleFS

FSInfo fs_info;  

WebSocketsServer webSocket = WebSocketsServer(81);

// The neat way to access settingsfile in FS
struct structSettings 
{
  uint32_t  nrReboots;
  boolean   skipChecksum;
  int       anotherInteger;
  char      someBytes[12];
  boolean   state;
} settings;

char      logFileName[30] = {}; 
File      logFile;
uint32_t  flushLogTimer;
bool      recordStatus    = true;
bool      forceFlush      = false;
String    thisAP;
uint32_t  idleTimer, ledTimer, upTimer = millis();
uint32_t  nextGuiUpdate;
uint16_t  smYear, smMonth, smDay, smHour, smMinute, smSecond;
bool      showMenu        = true;
uint32_t  fwVersion       = 0;
uint8_t   btnState  = 0;
uint8_t   pressType = 0;
uint32_t  interTimer, bounceTimer, buttonTimer, aliveTimer;
uint32_t  showBtnTimer;

#endif

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
