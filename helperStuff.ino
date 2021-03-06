/*
***************************************************************************  
**  Program : helperStuff
**
**  Copyright (c) 2021 Willem Aandewiel
**
**  TERMS OF USE: MIT License. See bottom of file.                                                            
***************************************************************************      
*/



//===========================================================
void timeStamp2Regs(String timeStamp)
{
  smYear    = 2000 + timeStamp.substring(0, 2).toInt();
  smMonth   = timeStamp.substring(2, 4).toInt();
  smDay     = timeStamp.substring(4, 6).toInt();
  smHour    = timeStamp.substring(6, 8).toInt();
  smMinute  = timeStamp.substring(8, 10).toInt();
  smSecond  = timeStamp.substring(10, 12).toInt();
  
} // encodeTimestamp


//===========================================================
uint32_t fwVersion2Uint32()
{ 
  int       tmpInt    = String(_FW_VERSION).indexOf(' ');
  String    sPart     = String(_FW_VERSION).substring(1, tmpInt);
  int       firstDot  = sPart.indexOf('.');  
  int       secondDot = sPart.indexOf('.', (firstDot+1));  
  String    sMajor    = sPart.substring(0, firstDot);
  String    sMinor    = sPart.substring((firstDot+1), secondDot);
  String    sSub      = sPart.substring((secondDot+1));
  uint32_t  iVersion  = (sMajor.toInt()   * 10000000) 
                        + (sMinor.toInt() * 10000)
                        + (sSub.toInt()   * 1);
                        
  DebugTf("VERSION[%s]\r\n"     , _FW_VERSION);
  DebugTf("[%s][%s][%s][%s]\r\n", sPart.c_str()
                                , sMajor.c_str()
                                , sMinor.c_str()
                                , sSub.c_str());
  DebugTf("uint32[%d]\r\n"      , iVersion);

  return iVersion;
  
} // version2Uint32()




//==================================================================================
#define MAX_TLGRM_LENGTH  2000

void  printSlimmeMeterRaw()
{
  char    tlgrm[MAX_TLGRM_LENGTH] = "";
  char    checkSum[10]            = "";
   
  DebugTln("printSlimmeMeterRaw() ");

  int l = 0, lc = 0;

  if (l=readSerialUntil(TelnetStream, '/', 5000, false))
  {
    TelnetStream.print("\r\n/");
    l  = readSerialUntil(TelnetStream, '!', 1000, true);
    lc = readSerialUntil(TelnetStream, '\n',1000, true) +l;

  }
  else
  {
    DebugTln(F("RawMode: Timed-out - no telegram received within 5 seconds"));
    return;
  }

  DebugTf("Telegram has %d chars\r\n\n", lc);
  return;
  
} //  printSlimmeMeterRaw()


//===========================================================================================
int readSerialUntil(Stream &uartOut, char cEnd, uint32_t waitT, bool doEcho)
{
  uint32_t waitTimer = millis();
  int charCount = 0;
  char cIn;
  
  while((millis()-waitTimer) < waitT)
  {
    yield();
    while (Serial.available())
    {
      cIn=Serial.read();
      if ((cIn>=' '&& cIn<='z') || cIn=='\r' || cIn=='\n')
      {
        waitTimer = millis(); //-- rewind
        if (doEcho) uartOut.print(cIn);
        charCount++;
        if (cIn==cEnd)
        {
          //if (doEcho) uartOut.println();
          return charCount;
        }
        waitTimer = millis();
      }
    } // if available
    
  } // while still time

  return 0;

} //  readSerialUntil()


//=======================================================================
int8_t splitString(String inStrng, char delimiter, String wOut[], uint8_t maxWords)
{
  uint16_t inxS = 0, inxE = 0, wordCount = 0;
  inStrng.trim();
  while(inxE < inStrng.length() && wordCount < maxWords)
  {
    inxE  = inStrng.indexOf(delimiter, inxS);         //finds location of first ,
    wOut[wordCount] = inStrng.substring(inxS, inxE);  //captures first data String
    wOut[wordCount].trim();
    //DebugTf("[%d] => [%c] @[%d] found[%s]\n", wordCount, delimiter, inxE, wOut[wordCount].c_str());
    inxS = inxE;
    inxS++;
    wordCount++;
  }
  if (inxS < inStrng.length())
  {
    wOut[wordCount] = inStrng.substring(inxS, inStrng.length());  //captures first data String
    //DebugTf("[%d] rest => [%s]\n", wordCount, wOut[wordCount].c_str());
  }

  return wordCount;

} // splitString()


//===========================================================================================
void strConcat(char *dest, int maxLen, const char *src)
{
  if (strlen(dest) + strlen(src) < maxLen)
  {
    strcat(dest, src);
  }
  else
  {
    DebugTf("Combined string > %d chars\r\n", maxLen);
  }

} // strConcat()


//===========================================================================================
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
    DebugTf("Combined string > %d chars\r\n", maxLen);
  }

} // strConcat()


//===========================================================================================
void strConcat(char *dest, int maxLen, int32_t v)
{
  static char buff[25];
  sprintf(buff, "%d", v);

  if (strlen(dest) + strlen(buff) < maxLen)
  {
    strcat(dest, buff);
  }
  else
  {
    DebugTf("Combined string > %d chars\r\n", maxLen);
  }

} // strConcat()


//===========================================================================================
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
void strCopy(char *dest, int maxLen, const char *src, uint8_t frm, uint8_t to)
{
  int d=0;
  //DebugTf("dest[%s], src[%s] max[%d], frm[%d], to[%d] =>\r\n", dest, src, maxLen, frm, to);
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
char *intToStr(int32_t v)
{
  static char buff[25];
  sprintf(buff, "%d", v);
  return buff;

} // intToStr()

//===========================================================================================
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
float formatFloat(float v, int dec)
{
  return (String(v, dec).toFloat());

} //  formatFloat()

//===========================================================================================
float strToFloat(const char *s, int dec)
{
  float r =  0.0;
  int   p =  0;
  int   d = -1;

  r = strtof(s, NULL);
  p = (int)(r*pow(10, dec));
  r = p / pow(10, dec);
  //DebugTf("[%s][%d] => p[%d] -> r[%f]\r\n", s, dec, p, r);
  return r;

} //  strToFloat()



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
