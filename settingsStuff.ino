/*
***************************************************************************
**  Program  : settingsStuff, part of DSMRrecorder
**  Version  : v3.0
**
**  Copyright (c) 2020 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.
***************************************************************************
* 1.0.11 added Mindergas Authtoken setting
*/

//=======================================================================
void writeSettings()
{
  yield();
  Debug(F("Writing to ["));
  Debug(_SETTINGS_FILE);
  Debugln(F("] ..."));
  File file = FSYS.open(_SETTINGS_FILE, "w"); // open for reading and writing
  if (!file)
  {
    Debugf("open(%s, 'w') FAILED!!! --> Bailout\r\n", _SETTINGS_FILE);
    return;
  }
  yield();

  Debug(F("Start writing setting data "));

  file.print("nrReboots = ");
  file.println(settings.nrReboots);
  Debug(F("."));

  file.close();

  Debugln(F(" done"));
  Debugln(F("Wrote this:"));
  Debugf("nrReboots = [%u]\r\n", settings.nrReboots);
  Debugln();

} // writeSettings()


//=======================================================================
void readSettings(bool show)
{
  String sTmp, nColor;
  String words[10];

  File file;

  Debugf(" %s ..\r\n", _SETTINGS_FILE);

  if (!FSYS.exists(_SETTINGS_FILE))
  {
    Debugln(F(" .. file not found! --> created file!"));
    writeSettings();
  }

  for (int T = 0; T < 2; T++)
  {
    file = FSYS.open(_SETTINGS_FILE, "r");
    if (!file)
    {
      if (T == 0) Debugf(" .. something went wrong opening [%s]\r\n", _SETTINGS_FILE);
      else        Debugln(T);
      delay(100);
    }
  } // try T times ..

  Debugln(F("Reading settings:\r"));
  while(file.available())
  {
    sTmp      = file.readStringUntil('\n');
    sTmp.replace("\r", "");
    //Debugf("[%s] (%d)\r\n", sTmp.c_str(), sTmp.length());
    int8_t wc = splitString(sTmp.c_str(), '=', words, 10);
    words[0].toLowerCase();

    if (words[0].equalsIgnoreCase("nrReboots"))
      settings.nrReboots = words[1].toInt();

    if (words[0].equalsIgnoreCase("someBytes"))
      strCopy(settings.someBytes, sizeof(settings.someBytes), words[1].c_str());

  } // while available()

  file.close();

  Debugln(F(" .. done\r"));

} // readSettings()


//=======================================================================
void updateSetting(const char *field, const char *newValue)
{
  Debugf("-> field[%s], newValue[%s]\r\n", field, newValue);

  writeSettings();

} // updateSetting()


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
***************************************************************************/
