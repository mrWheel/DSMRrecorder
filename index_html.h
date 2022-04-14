static const char DSMRindex_html[] PROGMEM =
  R"(<html charset="UTF-8">
  <style type='text/css'>
    <!--font-size: 20px;-->
    table tbody tr td {
      font-size: 20px;
    }
    body {
      background-color: lightblue;
    }
  </style>
<!-- ------------------------------------------------------------ -->
     <body>
      <div class="header">
        <h1>
          <span id="devName">DSMRecorder</span> &nbsp; &nbsp; &nbsp;
          <span id="devVersion">[version]</span> &nbsp; &nbsp; &nbsp;
          <span id="devIPaddress" style='font-size: small;'>-</span> &nbsp;
        </h1>
       </div>
     <div>
      <form action = '' name='mainForm' method = 'get'>
         <table>
           <tr>
           <td style='width:120px;' valign='top'>Timing</td>
         </table>
      </form>
     </div>
     <br>
      <div>
        <form action='/update' method='GET'><big>Update Firmware </big>
          <input type='submit' class='button' name='SUBMIT' value='select Firmware' ENABLED/>
        </form>

        <form action='/FSmanager' method='GET'><big>FS manager </big>
          <input type='submit' class='button' name='SUBMIT' value='FS Manager' ENABLED/>
        </form>

        <form action='/ReBoot' method='POST'>ReBoot DSMRecorder 
          <input type='submit' class='button' name='SUBMIT' value='ReBoot'>
        </form>
      </div>
     <br>
<!-- ------------------------------------------------------------ -->
<script>
//"use strict";

  let webSocketConn;
  let needReload  = true;
  let singlePair;
  let onePair;

  window.onload=bootsTrap;
  window.onfocus = function() 
  {
    if (needReload) {
      window.location.reload(true);
    }
  };
    
  
  function bootsTrap() {
    let count = 0;
    while ( document.getElementById('devVersion').value == "[version]" ) {
      count++;
      console.log( "count ["+count+"] devVersion is ["+document.getElementById('devVersion').value+"]" );
      if (count > 10) {
        alert( "Count="+count+" => reload from server!" );
        window.location.reload(true);
      }
      setTimeout("", 500);
    }

  }; // bootsTrap()
  
  webSocketConn = new WebSocket( 'ws://'+location.host+':81/', ['arduino'] );
  console.log( "WebSocket('ws://"+location.host+":81/', ['arduino']) " );
  
  webSocketConn.onopen    = function () 
  { 
    console.log( "Connected!" );
    webSocketConn.send('Connect ' + new Date()); 
    console.log( "getDevInfo" );
    needReload  = false;

  }; 
  
  webSocketConn.onclose     = function () 
  { 
    console.log( " " );
    console.log( "Disconnected!" );
    console.log( " " );
    needReload  = true;
    let redirectButton = "<p></p><hr><p></p><p></p>"; 
    redirectButton    += "<style='font-size: 50px;'>Disconnected from DSMRecorder"; 
    redirectButton    += "<input type='submit' value='re-Connect' "; 
    redirectButton    += " onclick='window.location=\"/\";' />  ";     
    redirectButton    += "<p></p><p></p><hr><p></p>"; 
    document.getElementById( "Actual" ).innerHTML = redirectButton;

  }; 
  webSocketConn.onerror   = function (error)  
  { 
    console.log( "Error: " + error);
    console.log('WebSocket Error ', error);
  };
  webSocketConn.onmessage = function (e) 
  {
    needReload = false;
    parsePayload(e.data); 
  };

  function parsePayload(payload) 
  {
    console.log( "parsePayload["+payload+"]" );

    singlePair   = payload.split("," );
    var msgType  = singlePair[0].split("=");
    if (msgType[1] == "devInfo" ) {
        console.log("devInfo: "+devIPaddress+","+devVersion);
        for ( var i = 1; i < singlePair.length; i++ ) {
          onePair = singlePair[i].split("=" );
          console.log("set["+onePair[0].trim()+"] to["+onePair[1].trim()+"]" );
          document.getElementById(onePair[0].trim()).innerHTML = onePair[1].trim();
          document.getElementById(onePair[0].trim()).setAttribute('value',onePair[1].trim());
        }
    }

    if (msgType[1] == "timeStamp" ) 
    {
        console.log("timeStamp: "+payload);
        for ( var i = 1; i < singlePair.length; i++ ) 
        {
          onePair = singlePair[i].split("=");
          console.log("set["+onePair[0].trim()+"] to["+onePair[1].trim()+"]" );
        }
    }
  };

  
  function validateField(field) 
  {
    console.log(" validateField(): ["+field+"]" );
    if (field == "skipChecksum")
    {
      var nField = document.getElementById(field).checked;  
      console.log("setChecksum:newChecksum="+nField);
      webSocketConn.send("setChecksum:newChecksum="+nField);
    }
    else
    {
      console.log("not skipChecksum");
    }
    
  };  // validateField()
  
</script>
     </body></html>)";
