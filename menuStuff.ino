/*
 * Menu
*/


//===========================================================
char getInput()
{
  char cIn = 0;
  
  if (TelnetStream.available())
  {  
    cIn = TelnetStream.read();
    if (cIn == '\r' || cIn == '\n')
    {
      while(TelnetStream.available()) 
      {
        TelnetStream.read();
        yield();
      }
      showMenu = true;
      return 0;
    }
    if ((cIn >= 'A' && cIn <= 'Z') || (cIn >= 'a' && cIn <= 'z'))
    {
      while(TelnetStream.available()) 
      {
        TelnetStream.read();
        yield();
      }
      return cIn;
    }
    yield();
  }

  return 0;
  
} //  getInput()


//===========================================================
void handleInput(char cmd)
{
  switch(cmd)
  {
    case 's':
        TelnetStream.println("\r\n(Re)Start recording\r\n");
        recordStatus = true;
        TelnetStream.flush();
        break;
        
    case 'S':
        TelnetStream.println("\r\nStop recording\r\n");
        recordStatus = false;
        TelnetStream.flush();
        break;
        
    case 'f':
    case 'F':
        TelnetStream.println("\r\nFlush recording\r\n");
        forceFlush = true;
        TelnetStream.flush();
        break;
        
    case 'Q':
        delay(2000);
        TelnetStream.println("\r\nRestart DSMRecorder .. please wait..\r\n");
        TelnetStream.flush();
        delay(2000);
        ESP.restart();
        delay(5000);
        break;

    case 'Z':
        settings.nrReboots = 0;
        writeSettings();
        break;
          
  } // switch
  yield();

} //  handleInput()


//===========================================================
void menu(bool full=false)
{
  TelnetStream.println();
  TelnetStream.printf("firmware Version [%d]\r\n", fwVersion);
  TelnetStream.printf("       Recording [%s]\r\n", (recordStatus?"Yes":"No"));
  TelnetStream.printf("   nr of Reboots [%d]\r\n", settings.nrReboots);
  TelnetStream.println();
  TelnetStream.println("*s - (re)Start recording");
  TelnetStream.println("*S - Stop recording");
  TelnetStream.println(" F - Flush recording");
  TelnetStream.println("*Q - Quit Debug; restart DSMRecorder");
  TelnetStream.println("*Z - Zero number reboots");
  TelnetStream.print("-> ");
  showMenu = false;
  yield();
  
} // menu()


//===========================================================
void loopMenu()
{
  httpServer.handleClient();

  if (showMenu)
  {
    menu(false);
  }
  handleInput(getInput());
  if ((int)(millis() - ledTimer) > 0)
  {
    ledTimer = millis() + 500;
  }

  yield();
  
} //  loopMenu()

/* eof */
