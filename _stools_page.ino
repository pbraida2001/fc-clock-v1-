//******************************************************************************************************************
// Created by Pedro Braida Neto
// 01/31/2022 All Rights Reserved
//******************************************************************************************************************

//******************************************************************************************************************
// INDEX.HTML PAGE
//******************************************************************************************************************
const char stools_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
  <html>
  <head>
    <title>DBX-Digital Led</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      * {
        box-sizing: border-box;
      }
      body {
        font-family: "Lato", sans-serif;
        background: #f1f1f1;        
      }
      .container {
        display: inline-block;
        cursor: pointer;
        margin: 20px 20px;
      }
      .bar1, .bar2, .bar3 {
        width: 35px;
        height: 5px;
        background-color: #333;
        margin: 6px 0;
        transition: 0.4s;
      }
      .change .bar1 {
        -webkit-transform: rotate(-45deg) translate(-9px, 6px);
        transform: rotate(-45deg) translate(-9px, 6px);
      }
      .change .bar2 {opacity: 0;}
      .change .bar3 {
        -webkit-transform: rotate(45deg) translate(-8px, -8px);
        transform: rotate(45deg) translate(-8px, -8px);
      }      
      .sidebar {
        height: 100%;
        width: 0;
        position: fixed;
        z-index: 1;
        top: 0;
        right: 0;
        background-color: #1D8348;
        overflow-x: hidden;
        transition: 0.5s;
        padding-top: 60px;
      }
      .sidebar a {
        padding: 8px 8px 8px 32px;
        text-decoration: none;
        font-size: 21px;
        color: white;
        display: block;
        transition: 0.3s;
      }
      .sidebar a:hover {
        color: black;
      }
      .sidebar .closebtn {
        position: absolute;
        top: 0;
        right: 25px;
        font-size: 36px;
        margin-left: 50px;
      }
      #main {
        transition: margin-right .5s;
        padding: 16px;
      }      
      .logo {
        padding: 13px;
        float: left;
        width: 78%;     
        text-align: left;           
      }
      .logo_1 {
        padding: 13px;
        float: right;
        width: 90%;     
        text-align: right;           
      }
      .logo_2 {
        padding: 4px;
        float: center;
        width: 100%;     
        text-align: center;           
      } 
      .header {
        padding: 5px;
        background: white;
        margin-top: 0px;
        position: relative;
        display: flex;
        align-items: center;
      }
      .texto_1 {
        float: left;
        font-size: 50px;
        width: 100%; 
        text-align: center;
      }  
      .texto_2 {
        padding: 10px;
        font-size: 25px;
        text-align: center;
        color: white;
        background: #1D8348;
      }
      .texto_3 {
        padding: 20px;
        font-size: 25px;
        text-align: center;
      } 
      .texto_4 {
        float: right;
        font-size: 50px;
        width: 48%; 
        text-align: center;
      }
      .texto_5 {
        color: white;
        font-size: 20px;
        text-align: center;
      }       
      .footer {
        padding: 1px;
        text-align: center;
        background: #1D8348;
        margin-top: 15px;
      }
      .row:after {
        content: "";
        display: table;
        clear: both;
      } 
      .rightcolumn_1 {
        float: left;
        width: 20.0%;
        padding-left: 20px;
      }         
      .card {
        background-color: white;
        padding: 20px;
        margin-top: 20px;
        margin-right: 20px;
        border-radius: 20px;
      }
      .fakeimg_1 {
        background-color: #A9DFBF;
        color:black;
        width: 100%;
        height: auto;
        padding: 20px;
        text-align: left;
        word-wrap: break-word;
        vertical-align:middle;
        font-weight: bold;
        font-size: 15px;
        border-radius: 20px;  
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);                     
      } 
      progress {
        float: center;
        border-radius: 7px; 
        width: 80%;
        height: 22px;
        box-shadow: 1px 1px 4px rgba( 0, 0, 0, 0.2 );
      }
      progress::-webkit-progress-bar {
        background-color: white;
        border-radius: 7px;
      }
      progress::-webkit-progress-value {
        background-color: red;
        border-radius: 7px;
      }
      progress::-moz-progress-bar {
        /* style rules */
      }          
      label {
        float: left;
        text-align: center;  
        background-color: black;
        color: white;
        font-size: 20px;
        padding: 14px 20px;
        margin: 8px 0;
        border: none;
        cursor: pointer;
        width: 100%;
        border-radius: 10px;
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);  
      } 
      label:active {
        background-color: #4682B4;
        color: white;
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0), 0 6px 20px 0 rgba(0, 0, 0, 0);
        transform: translateY(4px);        
      }  
      input[type=submit] {
        float: left;
        text-align: center;  
        background-color: black;
        color: white;
        font-size: 20px;
        padding: 14px 20px;
        margin: 8px 0;
        border: none;
        cursor: pointer;
        width: 100%;
        border-radius: 10px;
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);  
      } 
      input[type=submit]:active {
        background-color: #4682B4;
        color: white;
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0), 0 6px 20px 0 rgba(0, 0, 0, 0);
        transform: translateY(4px);        
      }        
      input[type=file] {
        display: none;
      } 
      .container_1 {
        padding: 16px;
        text-align: center;
      }
      .modal {
        display: none;
        position: fixed;
        z-index: 1; 
        left: 30%;
        top: 30%;
        width: 40%; 
        height: auto;
        overflow: auto;
        background-color: #474e5d;
        padding-top: 40px;
        padding-bottom: 40px;
        border-radius: 10px;
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);  
      }
      .modal-content {
        background-color: #fefefe;
        margin: 1% auto 1% auto; 
        border: 1px solid #888;
        width: 80%; 
        border-radius: 10px;
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);  
      } 
      hr {
        border: 1px solid #f1f1f1;
        margin-bottom: 25px;
      }
      .close {
        position: absolute;
        right: 35px;
        top: 15px;
        font-size: 40px;
        font-weight: bold;
        color: #f1f1f1;
      }
      .close:hover,.close:focus {
        color: #f44336;
        cursor: pointer;
      }
      .clearfix::after {
        content: "";
        clear: both;
        display: table;
      }
      .container_1 {
        padding: 16px;
        text-align: center;
      }      
      button {
        background-color: black;
        color: white;
        font-size: 20px;
        padding: 14px 20px;
        margin: 8px 0;
        border: none;
        cursor: pointer;
        width: 100%;
        border-radius: 10px;
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);  
      }
      button:active {
        background-color: #4682B4;
        color: white;
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0), 0 6px 20px 0 rgba(0, 0, 0, 0);
        transform: translateY(4px);        
      }
      .cancelbtn, .deletebtn {
        float: center;
        width: 40%;
      }
      .cancelbtn {
        background-color: #ccc;
        color: black;
      }
      .deletebtn {
        background-color: #f44336;
      }         
      img {
        max-width: 100%;
        height: auto;
      }
      .espaco1{
        margin: 20px 0;
      }        
      .espaco2{
        margin: 30px 0;
      }      
      @media screen and (max-height: 1200pxpx) {
        .sidebar {padding-top: 15px;}
        .sidebar a {font-size: 18px;}
      }      
      @media screen and (max-width: 300px) {
        .cancelbtn, .deletebtn {
          width: 100%;
        }
      }                  
      @media screen and (max-width: 1199px) {
        .logo {
          float: center;
          width: 100%;
        }
        .logo_1 {
          float: center;
          width: 100%;
        }
        .logo_2 {
          float: center;
          width: 100%;
        }
        .fakeimg_1 {
          font-weight: normal;  
          font-size: 12px;
          height: auto;
        }
        .rightcolumn_1 {   
          width: 100%;
          padding: 0;
        }  
        .container {
          margin: 5px 5px;                     
        }       
      }
      @media screen and (max-width: 800px) {
        .texto_1 {
          font-size: 15px;
        }                                                           
      }
    </style>
  </head>
  <body>
    <div id="mySidebar" class="sidebar">
      <a href="javascript:void(0)" class="closebtn" onclick="closeNav()">x</a>
      <a href="/">Setup</a>
      <a href="/stools">Tools</a>
    </div>
    <div id="main">
      <div class="header">
        <div class="logo">
          <a href="/">
          <img src="logo" width="213" height="90">
          </a>
        </div>  
        <div class="texto_4">
          <div class="logo_2">
            <img src="eng" width="94" height="90">
          </div>  
        </div>
        <div class="logo_1">
          <a href="/stools">
          <img src="logo" width="213" height="90">
          </a>
        </div> 
        <div>
          <div id="mh" class="container" onclick="openNav(this)">
            <div class="bar1"></div>
            <div class="bar2"></div>
            <div class="bar3"></div>
          </div>
        </div>
      </div>  
      <div>
        <div class="texto_2">
          System Tools $ACCO_LOC$
        </div>
      </div> 
      <div class="row">  
        <div class="rightcolumn_1">
          <div class="card">
            <div class="texto_3">            
              Update Firmware
            </div>
            <div class="fakeimg_1">
              <div class="row">
                <form method='POST' action='/doUpdate' enctype='multipart/form-data'>
                  <label for='input_1'>Choose File</label>              
                  <input type="file" id="input_1" name="update">
                  <div id="id01" class="modal">
                    <span onclick="document.getElementById('id01').style.display='none'" class="close" title="Close Modal">X</span>
                    <div class="container_1">
                      <div class="modal-content">
                        <h1>Update Firmware</h1>
                        <p>Are you sure you want to update the firmware ?</p>
                        <p>This operation takes approximately 1 minute and cannot be stopped !</p> 
                        <p>Please keep this window open until the Gateway LED stops flashing</p>                      
                        <div class="clearfix">
                          <button type="button" name="cancel1_cfg" onclick="document.getElementById('id01').style.display='none'" class="cancelbtn">No</button>
                          <button type="submit" id="bupdate" class="deletebtn" >Yes</button>                                               
                        </div>                        
                      </div> 
                      <div class="espaco1" id="progress1" $SHOWA$>
                      </div> 
                      <div class="row" id="progress3" $SHOWA$> 
                        <div class="texto_5">
                          <span id="prgval"></span>                                                      
                        </div>                      
                      </div> 
                      <div class="espaco2" id="progress1" $SHOWA$>
                      </div>                                              
                      <div class="row" id="progress2" $SHOWA$> 
                        <progress id="progress" value="0" max="100"></progress>
                      </div>
                      <div class="espaco1" id="progress1" $SHOWA$>
                      </div>                      
                      <div class="texto_5">
                        <span id="prgval1"></span>                                                      
                      </div>                                                                      
                    </div>                  
                  </div>
                </form>                
              </div>             
            </div>           
          </div>
        </div>
        <div class="rightcolumn_1">
          <div class="card">
            <div class="texto_3">            
              Reboot System
            </div>
            <div class="fakeimg_1">
              <div class="row">
                <form action="/apreset.php">
                  <label onclick="document.getElementById('id02').style.display='block'">Reboot</label>                   
                  <div id="id02" class="modal">
                    <span onclick="document.getElementById('id02').style.display='none'" class="close" title="Close Modal">X</span>               
                    <div class="container_1">
                      <div class="modal-content">
                        <h1>Reboot System</h1>
                        <p>Are you sure you want to Reboot System ?</p> 
                        <p>This operation takes approximately 30 seconds and cannot be stopped !</p> 
                        <p>Please wait...</p>                      
                        <div class="clearfix">
                          <button type="button" name="cancel2_cfg" onclick="document.getElementById('id02').style.display='none'" class="cancelbtn">No</button>
                          <button type="submit" id="bupdate" class="deletebtn" >Yes</button>                                               
                        </div>
                      </div>                   
                    </div>
                  </div>
                </form>           
              </div>
            </div>
          </div>
        </div>             
      </div>
      <div class="footer">
        <div class="texto_2">
          Flex Control
        <div>
      </div>
    </div>
    <script> 
      var gateway=`ws://${window.location.hostname}/ws`;
      var websocket;
      window.addEventListener('load',onload);
      function onload(event) {
        initWebSocket();
      }
      function getReadings(){
      }      
      function initWebSocket() {
        websocket=new WebSocket(gateway);
        websocket.onopen=onOpen;
        websocket.onclose=onClose;
        websocket.onmessage=onMessage;
      }
      function onOpen(event) {
      }
      function onClose(event) {
        setTimeout(initWebSocket,2000);
      }
      function onMessage(event) {
        var myObj=JSON.parse(event.data);
        var keys=Object.keys(myObj); 
        const progressElement=document.getElementById("progress");    
        if(keys[0]=="pgval") {
          document.getElementById("progress1").style.display="block"; 
          document.getElementById("progress2").style.display="block"; 
          document.getElementById("progress3").style.display="block";                   
          var key=keys[0];
          progressElement.value+=3;
          document.getElementById("prgval").innerHTML=progressElement.value+" %";
          if(progressElement.value>=99) document.getElementById("prgval1").innerHTML="Wait until device reboot...";
        }                     
      }                     
      const fileInput=document.getElementById('input_1'); 
      var modal_1=document.getElementById('id01');
      var modal_2=document.getElementById('id02');
      var modal_3=document.getElementById('id03');
      var modal_4=document.getElementById('id04');
      var modal_5=document.getElementById('id05');      
      var x=document.getElementById('bupdate');
      const handleFiles =()=>{        
        var selectedFiles=fileInput.files[0];
        if(selectedFiles.name!="") {
          modal_1.style.display="block";   
          x.style.background="#f44336";
          x.disabled=false;     
        }
      }            
      fileInput.addEventListener("change",handleFiles); 
      window.onbeforeunload=function () {
        x.style.background="black";
        x.disabled=true;        
      }         
      window.onclick=function(event) {
        if(event.target==modal_1) {
          modal_1.style.display="none";
        } else if (event.target==modal_2) {
          modal_2.style.display="none";
        } else if (event.target==modal_3) {
          modal_3.style.display="none"; 
        } else if (event.target==modal_4) {
          modal_4.style.display="none"; 
        } else if (event.target==modal_5) {
          modal_5.style.display="none";          
        }        
      }
      function st_bt1(x) {
        let newloc="/log";
        window.location.assign(newloc);       
      }
      function st_bt2(x) {
        let newloc="/stemplog";
        window.location.assign(newloc);       
      }      
      function openNav(x) {
        x.classList.toggle("change");
        if(document.getElementById("mySidebar").style.width=="250px") {
          document.getElementById("mySidebar").style.width = "0";
          document.getElementById("main").style.marginRight= "0";         
        } else {
          document.getElementById("mySidebar").style.width = "250px";
          document.getElementById("main").style.marginRight = "250px";
        }
      }
      function closeNav() {
        document.getElementById("mh").classList.toggle("change");
        document.getElementById("mySidebar").style.width="0";
        document.getElementById("main").style.marginRight="0";
      }
    </script>  
  </body>
</html>
)rawliteral";

//******************************************************************************************************************
// FUNCTION PROCESSOR_UPDATE
//******************************************************************************************************************

String update_processor(const String& var) {
 
  return("");
  
}

//******************************************************************************************************************
