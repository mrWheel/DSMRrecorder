/*
***************************************************************************
**  Program  : webSocketEvent, part of DSMRecorder
**  Version  : v0.4.3
**
**  Copyright (c) 2019, 2020, 2021, 2022 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
*/


static String   prevTimestamp;
static bool     isConnected;
static int8_t   savMin = 0;
String wOut[15], wParm[10], wPair[4];


//===========================================================================================
void webSocketEvent(uint8_t wsClient, WStype_t type, uint8_t *payload, size_t lenght)
{
  String  wsPayload = String((char *) &payload[0]);
  char   *wsPayloadC = (char *) &payload[0];
  String  wsString;

  handleFlashButton();

  switch(type)
  {
    case WStype_DISCONNECTED:
      DebugTf("[%u] Disconnected!\n", wsClient);
      isConnected = false;
      break;

    case WStype_CONNECTED:
    {
      IPAddress ip = webSocket.remoteIP(wsClient);
      if (!isConnected)
      {
        DebugTf("[%u] Connected from %d.%d.%d.%d url: %s\n", wsClient, ip[0], ip[1], ip[2], ip[3], payload);
        isConnected = true;
        webSocket.sendTXT(wsClient, "{\"msgType\":\"ConnectState\",\"Value\":\"Connected\"}");
      }

    }
    break;

    case WStype_TEXT:
      DebugTf("[%u] Got message: [%s]\n", wsClient, payload);
      String FWversion = String(_FW_VERSION);

      if (wsPayload.indexOf("getDevInfo") > -1)
      {
        wsString  = "";
        wsString += ", devName=" + String(_HOSTNAME);
        wsString += ", devIPaddress=" + WiFi.localIP().toString() ;
        wsString += ", devVersion=[" + FWversion.substring(0, (FWversion.indexOf('(') -1)) + "]";
        //wsString += ", actYear=" + String(actYear);
        //wsString += ", actMonth=" + String(actMonth);
        //wsString += ", actDay=" + String(actDay);
        //wsString += ", actHour=" + String(actHour);
        //wsString += ", actSpeed=" + String(actSpeed);

        DebugTln(wsString);
        webSocket.sendTXT(wsClient, "msgType=devInfo" + wsString);

      }
      if (wsPayload.indexOf("getSkipChecksum") > -1)
      {
        wsString  = "";
        wsString += ", skipChecksum=";
        if (settings.skipChecksum)  wsString += "true";
        else                        wsString += "false";

        DebugTln(wsString);
        webSocket.sendTXT(wsClient, "msgType=skipChecksum" + wsString);

      }
      else if (wsPayload.indexOf("runMode") > -1)
      {
        int8_t wc = splitString(wsPayload.c_str(), ':', wOut, 14);
        DebugTf("wOut[1] => [%s]\r\n", wOut[1].c_str());
        wc = splitString(wOut[1].c_str(), ',', wParm, 10);
        for(int f=0; f <wc; f++)
        {
          //Debugf("wParm[%d] => [%s]\n", f, wParm[f].c_str());
          splitString(wParm[f].c_str(), '=', wPair, 4);
          int runMode; //-- make this something global
          Debugf("runMode >> wParm[%d][%s] : Field[%s] => Value[%s]\n", f, wParm[f].c_str(), wPair[0].c_str(), wPair[1].c_str());
          if (wPair[1] == "doNormal")       runMode = 1;
          else if (wPair[1] == "doHours")   runMode = 2;
          else if (wPair[1] == "doDays")    runMode = 3;
          else if (wPair[1] == "doMonths")  runMode = 4;
          else                              runMode = 0;
          Debugf("runMode [%s] set to [%d]\n", wPair[1].c_str(), runMode);
        }

      }
      else if (wsPayload.indexOf("recordStatus") > -1)
      {
        int8_t wc = splitString(wsPayload.c_str(), ':', wOut, 14);
        DebugTf("wOut[1] => [%s]\r\n", wOut[1].c_str());
        wc = splitString(wOut[1].c_str(), ',', wParm, 10);
        for(int f=0; f <wc; f++)
        {
          //Debugf("wParm[%d] => [%s]\n", f, wParm[f].c_str());
          splitString(wParm[f].c_str(), '=', wPair, 4);
          Debugf("recordStatus >> wParm[%d][%s] : Field[%s] => Value[%s]\n", f, wParm[f].c_str(), wPair[0].c_str(), wPair[1].c_str());
          if (wPair[1] == "doStart")    recordStatus = 1;
          else                          recordStatus = 0;
          Debugf("recordStatus [%s] set to [%d]\n", wPair[1].c_str(), recordStatus);
        }

      }
      else if (wsPayload.indexOf("setTiming") > -1)
      {
        int8_t wc = splitString(wsPayload.c_str(), ':', wOut, 14);
        DebugTf("wOut[1] => [%s]\n", wOut[1].c_str());
        wc = splitString(wOut[1].c_str(), '=', wParm, 10);
        DebugTf("wOut[1][%s] => [%s]=[%s]\n", wOut[1].c_str(), wParm[0].c_str(), wParm[1].c_str());
        //if (String(actTiming) != wParm[0])
        //{
        //  recordStatus = 0;  // stop running!
        //  sprintf(actTiming, "%s", wParm[1].c_str());
        //  DebugTf("Timing changed to [%s]\n", actTiming);
        //  writeSettings();
        //}

      }
      else if (wsPayload.indexOf("setDSMR") > -1)
      {
        int8_t wc = splitString(wsPayload.c_str(), ':', wOut, 14);
        DebugTf("wOut[1] => [%s]\n", wOut[1].c_str());
        wc = splitString(wOut[1].c_str(), '=', wParm, 10);
        //DebugTf("wOut[1][%s] => [%s]=[%s]\n", wOut[1].c_str(), wParm[0].c_str(), wParm[1].c_str());
        //if (String(actDSMR) != wParm[0])
        //{
        //  recordStatus = 0;  // stop running!
        //  sprintf(actDSMR, "%s", wParm[1].c_str());
        //  DebugTf("DSMR standaard changed to [%s]\n", actDSMR);
        //  writeSettings();
        //}
        //DebugTf("DSMR standaard [%s]\n", actDSMR);

      }
      else if (wsPayload.indexOf("setChecksum") > -1)
      {
        int8_t wc = splitString(wsPayload.c_str(), ':', wOut, 14);
        DebugTf("wOut[1] => [%s]\n", wOut[1].c_str());
        wc = splitString(wOut[1].c_str(), '=', wParm, 10);
        //DebugTf("wOut[1][%s] => [%s]=[%s]\n", wOut[1].c_str(), wParm[0].c_str(), wParm[1].c_str());
        if (wParm[1] == "true")
              settings.skipChecksum = true;
        else  settings.skipChecksum = false;
        writeSettings();

      }

      break;

  } // switch(type)

} // webSocketEvent()


//==================================================================================================
void updateGUI()
//==================================================================================================
{
  String wsString;
/*
  sprintf(cTimeStamp, "%04d-%02d-%02d %02d:%02d:%02d", year(), month(), day(), hour(), minute(), second());
*/
  wsString  = "";
/*
  if (useNTPtiming)
    wsString += ", actTiming=NTP";
  else  wsString += ", actTiming=INTERN";
*/
  //wsString += ", timeStamp=" + String(cTimeStamp);
  wsString += ", timeStamp=12:22:31";
  if (recordStatus == 1) wsString += ", recordStatus=1";
  else                   wsString += ", recordStatus=0";
  //DebugTf("msgType=timeStamp %s\r\n", wsString.c_str());
  nextGuiUpdate = millis() + 5000;

  webSocket.broadcastTXT("msgType=timeStamp" + wsString);

} // updateGUI()
