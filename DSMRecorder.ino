/*
***************************************************************************  
 *  Program  : DSMRecorder
 */
#define _FW_VERSION "v0.0.1 (14-04-2022)"
/* 
***************************************************************************  
*  Board:              "Generic ESP8266 Module"
*  Builtin Led:        "2"
*  CPU Frequency:      "160 MHz / 80 MHz"
*  Crystal Frequency:  "26MHz"
*  Flash Size:         "4MB (FS:2MB OTA:~1019KB)"
*  Flash Frequency:    "40MHz"
*  Reset Method: setup dependent  
*  Debug port:         "Disabled"
*  Espressif FW:       "nonos-sdk 2.2.1+100 (190703)" 
***************************************************************************  
*  You need to install the Arduino esp8266 core:
*  - tested with 2.7.4
*  
*  You need to install the following libraries:
*  
*  - https://github.com/jandrassy/TelnetStream
*
***************************************************************************  
*  Pin13    ->   From P1
***************************************************************************  
*/

#define _HOSTNAME         "DSMRecorder"

#define _WIFI_AP_ON      // will always start AP with IPAddress 10.10.10.10.
//#define _WIFI_STA_ON     // will always start WiFi as Station

#include "DSMRecorder.h"


//===========================================================
void openLogFile() 
{
  char tmp1FileName[30] = {};
  char tmp2FileName[30] = {};
  int  fNr = 1;
  File logF;

  snprintf(tmp1FileName, sizeof(tmp1FileName), "/log-%03d.txt", 1);
/*
  if (!FSYS.exists(tmp1FileName))
  {
    DebugTf("logFile [%s] new file!\r\n", tmp1FileName);
    logF = FSYS.open(tmp1FileName, "w");
    logF.println("mooi bestand");
    logF.close();
  }
  else
*/
  {
    //-- move up
    for(int x=_MAX_LOG_FILES; x>1; x--)
    {
      snprintf(tmp1FileName, sizeof(tmp1FileName), "/log-%03d.txt", x);
      snprintf(tmp2FileName, sizeof(tmp2FileName), "/log-%03d.txt", x-1);
      FSYS.remove(tmp1FileName);
      DebugTf("move [%s] to [%s]\r\n", tmp2FileName, tmp1FileName);
      if (FSYS.exists(tmp2FileName))
      {
        FSYS.rename(tmp2FileName, tmp1FileName);
      }
    }
  }
  snprintf(tmp1FileName, sizeof(tmp1FileName), "/log-%03d.txt", 1);
  DebugTf("create logFile [%s]!\r\n", tmp1FileName);
  logF = FSYS.open(tmp1FileName, "w");

  DebugFlush();

  
} //  openLogFile()


//===========================================================
void setup() 
{
  delay(100);
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(_DTR_ENABLE, OUTPUT);
  
  Serial.begin(115200); //, SERIAL_8N1);  //-- slimmeMeter (after swap())
  while(!Serial) { /* wait a bit */ delay(10); }
  
  Debugln("\r\n");
  Debugf("%s started ...\n", _HOSTNAME);
  Debugf("FW version: %s\r\n\n", _FW_VERSION);
  Serial.flush();

  fwVersion = fwVersion2Uint32();

  LittleFS.begin();
  readSettings(true);
  settings.nrReboots++;
  writeSettings();

    
    startWiFi(_HOSTNAME, 240);
    
    startTelnet();

    setupFS();
    /**
    //--------------------------------------------------------------
    httpServer.on("/",       indexHtml);
    httpServer.on("/index",  indexHtml);
    httpServer.on("/update", updateFirmware);
    httpServer.onNotFound([]() 
    {
      indexHtml();
    });
    **/
    //--------------------------------------------------------------

    httpServer.begin();
    Debugln("HTTP Server started");
      
#if defined( _WIFI_AP_ON ) 
    Debugf("\r\nConnect to AP %s .. \r\n", thisAP.c_str());
    Debugf(".. and use \"telnet %s\" for debugging ..\r\n\n", WiFi.softAPIP().toString().c_str());
#endif
#if defined( _WIFI_STA_ON ) 
    Debugf("\r\nConnect to SSID %s .. \r\n", WiFi.SSID().c_str());
    Debugf(".. and use \"telnet %s\" for debugging ..\r\n\n", WiFi.localIP().toString().c_str());
#endif

  openLogFile();
  snprintf(logFileName, sizeof(logFileName), "/log-%03d.txt", 1);
  logFile = FSYS.open(logFileName, "w");
  flushLogTimer = millis();
 
  Debugln("\r\nSerial connected to slimmeMeter from here on..");
  Debugln("Monitor with telnet after this point!\r\n");
  DebugFlush();
  yield();

  //-- swap RX(1) & TX(3) to RX(13) & TX(15)
  //-- GPIO13 (pin7) is receive (RX) from SlimmeMeter
  //-- otherwise ESP8266 won't flash
  Serial.end();
  delay(100);
  Serial.begin(115200, SERIAL_8N1);
  Serial.swap();
  serialSwapped = true;
  digitalWrite(_DTR_ENABLE, HIGH);
  
} //  setup()


//===========================================================
void loop() 
{
  httpServer.handleClient();
  handleFlashButton();

  if (forceFlush || (millis() - flushLogTimer) > 60000)
  {
    flushLogTimer = millis();
    forceFlush    = false;
    logFile.close();
    LittleFS.info(fs_info);
    DebugTf("Total Bytes [%10.10s]\r\n", formatBytes(fs_info.totalBytes).c_str()); 
    DebugTf("Used Bytes  [%10.10s]\r\n", formatBytes(fs_info.usedBytes).c_str()); 
    int32_t freeBytes = (int)(fs_info.totalBytes - fs_info.usedBytes);
    DebugTf("FreeBytes   [%10.10s]\r\n", formatBytes(freeBytes).c_str());
    if (freeBytes < 100000) 
    {
      DebugTln("Filesystem full! No more logging");
      recordStatus = false;
    }
    else
    {
      logFile = FSYS.open(logFileName, "a");
      logFile.println("\r\n-----flush-----\r\n");
    }
    DebugTf("[%s] flushed\r\n", logFileName);
  }
  
  if (showMenu)
  {
    menu(true);
  }
  handleInput(getInput());

  if ((int)(millis()-ledTimer) > 2000)
  {
    ledTimer = millis();
    digitalWrite(BUILTIN_LED, !digitalRead(BUILTIN_LED));
  }

  while(Serial.available())
  {
    char tIn = Serial.read();
    if (recordStatus) logFile.print(tIn);    
  }
  
} //  loop()

/* eof*/
