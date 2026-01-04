//******************************************************************************************************************
// Created by Pedro Braida Neto
// 01/31/2022 All Rights Reserved
//******************************************************************************************************************

//******************************************************************************************************************
// CONFIGURACAO DE REDE.HTML PAGE
//******************************************************************************************************************
const char index_html[] PROGMEM = R"rawliteral(
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
        width: 33.3%;
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
        font-weight: normal; 
        font-size: 20px;
        border-radius: 20px;  
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);                   
      }  
      .fakeimg_2 {
        background-color: #A9DFBF;
        color:black;
        width: 100%;
        height: auto;
        padding: 20px;
        text-align: left;
        word-wrap: break-word;
        vertical-align:middle;
        font-weight: normal; 
        font-size: 20px;
        border-radius: 20px; 
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);          
      }  
      .fakeimg_3 {
        background-color: #A9DFBF;
        color:black;
        width: 100%;
        height: auto;
        padding: 20px;
        text-align: left;
        word-wrap: break-word;
        vertical-align:middle;
        font-weight: normal; 
        font-size: 20px;
        border-radius: 20px; 
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);          
      }  
      input[type=text],select,textarea {
        width: 98%;
        padding: 12px;
        border: 1px solid #ccc;
        border-radius: 10px;
        resize: vertical;
        font-size: 15px;
        max-lenght: 120;
      }
      input[type=number] {
        width: 98%;
        padding: 12px;
        border: 1px solid #ccc;
        border-radius: 10px;
        resize: vertical;
      }
      input[type=submit] {
        background-color: black;
        color: white;
        padding: 10px 10%;
        border: none;
        border-radius: 10px;
        cursor: pointer;
        margin: 8px 5px;
        margin-right: 20px;
        float: right;
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);
      }
      input[type=submit]:active {
        background-color: #4682B4;
        color: white;
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0), 0 6px 20px 0 rgba(0, 0, 0, 0);
        transform: translateY(4px);
      }
      .col-25 {
        float: left;
        width: 40%;
        margin-top: 5px;
      }
      .col-75 {
        float: left;
        width: 60%;
        margin-top: 5px;
      }  
      .bcol-r {
        float: right;
        width: 90%;
        margin-top: 2px;
      } 
      img {
        max-width: 100%;
        height: auto;
      }
      .espaco{
        margin: 40px 0;
      }   
      @media screen and (max-height: 1199pxpx) {
        .sidebar {padding-top: 15px;}
        .sidebar a {font-size: 18px;}
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
        .fakeimg_1,.fakeimg_2,.fakeimg_3{
          font-weight: normal;  
          font-size: 18px;
          height: auto;
        }
        .rightcolumn_1 {   
          width: 100%;
          padding: 0;
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
          <a href="/">
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
          Network Settings
        </div>
      </div>    
      <div class="row">
        <div class="rightcolumn_1">
          <form action="/apnetcfg1.php">
            <div class="card">
              <div class="fakeimg_1">
                <div class="texto_3">            
                  Wi-Fi Settings
                </div>              
                <div class="row">
                  <div class="col-25">
                    <label for="wifiradio">Wi-Fi Radio</label>
                  </div>
                  <div class="col-75">
                    <select id="wifiradio" name="wifi_radio" onchange="wifi_show(this)">
                      <option value="radio_enable"$RADIO1_ST$>Enable</option>
                      <option value="radio_disable"$RADIO2_ST$>Disable</option>
                    </select>
                  </div> 
                </div>                        
                <div class="row" id="wifiid" $SHOW1$>                  
                    <div class="col-25">
                      <label for="wifissid">SSID</label>
                    </div>
                    <div class="col-75">
                      <input type="text" id="wifissid" name="wifissid_cfg" value="$WIFI_SSID$">
                    </div> 
                </div>                  
                <div class="row" id="wifipa" $SHOW1$>  
                    <div class="col-25">
                      <label for="wifipass">Wi-Fi Password</label>
                    </div>
                    <div class="col-75">
                      <input type="text" id="wifipass" name="wifipass_cfg" value="$WIFI_PASS$">
                    </div>
                </div>
                <div class="row" id="wifiap" $SHOW1$>  
                    <div class="col-25">
                      <label for="acppass">AP Password</label>
                    </div>
                    <div class="col-75">
                      <input type="text" id="acppass" name="acppass_cfg" value="$ACPT_PASS$">
                    </div>
                </div>
                <div class="row" id="wifiip" $SHOW1$>  
                    <div class="col-25">
                      <label for="wifiip">IP Address</label>
                    </div>
                    <div class="col-75">
                      <input type="text" id="wifiip" name="wifiip_cfg" value="$WIFI_IPAD$">
                    </div>
                </div>
                <div class="row" id="wifinm" $SHOW1$>  
                    <div class="col-25">
                      <label for="wifinm">Network Mask</label>
                    </div>
                    <div class="col-75">
                      <input type="text" id="wifinm" name="wifinm_cfg" value="$WIFI_NTMA$">
                    </div>
                </div>
                <div class="row" id="wifigw" $SHOW1$>  
                    <div class="col-25">
                      <label for="wifigw">Gateway IP Address</label>
                    </div>
                    <div class="col-75">
                      <input type="text" id="wifigw" name="wifigw_cfg" value="$WIFI_GATE$">
                    </div>
                </div>
                <div class="row" id="wifidn" $SHOW1$>  
                    <div class="col-25">
                      <label for="wifidn">DNS IP Address</label>
                    </div>
                    <div class="col-75">
                      <input type="text" id="wifidn" name="wifidn_cfg" value="$WIFI_DNS$">
                    </div>
                </div>                                                     
              </div> 
              <div class="bcol-r">
                <input type="submit" value=" Save ">
              </div> 
              <div class="espaco">
              </div>
            </div>
          </form>
        </div>
        <div class="rightcolumn_1">
          <form action="/apnetcfg3.php">
            <div class="card">
              <div class="fakeimg_3">
                <div class="texto_3">            
                  Comunications Port
                </div>
                <div class="row">  
                  <div class="col-25">
                    <label for="ethecp">UDP Port</label>
                  </div>
                  <div class="col-75">
                    <input type="text" id="ethecp" name="ethecp_cfg" value="$ETHE_CPORT$">
                  </div>
                </div>
                <div class="row">  
                  <div class="col-25">
                    <label for="wificp">TCP Port</label>
                  </div>
                  <div class="col-75">
                    <input type="text" id="wificp" name="wificp_cfg" value="$WIFI_CPORT$">
                  </div>
                </div> 
                <div class="texto_3">
                  UDP FeedBack
                </div>
                <div class="row">  
                  <div class="col-25">
                    <label for="fbip1">IP1 Target</label>
                  </div>
                  <div class="col-75">
                    <input type="text" id="fbip1" name="fbip_1" value="$FDIP_1$">
                  </div>
                </div>
                <div class="row">  
                  <div class="col-25">
                    <label for="fbpt1">Port1 Target</label>
                  </div>
                  <div class="col-75">
                    <input type="text" id="fbpt1" name="fbpt_1" value="$FDPT_1$">
                  </div>
                </div>               
                <div class="row">  
                  <div class="col-25">
                    <label for="fbip2">IP2 Target</label>
                  </div>
                  <div class="col-75">
                    <input type="text" id="fbip2" name="fbip_2" value="$FDIP_2$">
                  </div>
                </div>
                <div class="row">  
                  <div class="col-25">
                    <label for="fbpt2">Port2 Target</label>
                  </div>
                  <div class="col-75">
                    <input type="text" id="fbpt2" name="fbpt_2" value="$FDPT_2$">
                  </div>
                </div>                
                <div class="row">  
                  <div class="col-25">
                    <label for="fbip3">IP3 Target</label>
                  </div>
                  <div class="col-75">
                    <input type="text" id="fbip3" name="fbip_3" value="$FDIP_3$">
                  </div>
                </div>
                <div class="row">  
                  <div class="col-25">
                    <label for="fbpt3">Port3 Target</label>
                  </div>
                  <div class="col-75">
                    <input type="text" id="fbpt3" name="fbpt_3" value="$FDPT_3$">
                  </div>
                </div>   
              </div> 
              <div class="bcol-r">
                <input type="submit" value=" Save ">
              </div> 
              <div class="espaco">
              </div>
            </div>             
          </form>
        </div>                
      </div> 
      <div class="footer">
        <div class="texto_2">
          Flex Control
        <div>
      </div>
    </div>
    <script>
      function wifi_show(x) {
        var selected=x.value;
        if(selected=="radio_disable") {
          document.getElementById("wifiid").style.display="none";
          document.getElementById("wifipa").style.display="none";
          document.getElementById("wifiap").style.display="none";
          document.getElementById("wifiip").style.display="none";
          document.getElementById("wifinm").style.display="none";
          document.getElementById("wifigw").style.display="none";
          document.getElementById("wifidn").style.display="none";                                                            
        } else {
          document.getElementById("wifiid").style.display="block";
          document.getElementById("wifipa").style.display="block";
          document.getElementById("wifiap").style.display="block";
          document.getElementById("wifiip").style.display="block";
          document.getElementById("wifinm").style.display="block";
          document.getElementById("wifigw").style.display="block";
          document.getElementById("wifidn").style.display="block"          
        }
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
        document.getElementById("mySidebar").style.width = "0";
        document.getElementById("main").style.marginRight= "0";
      }
    </script>  
  </body>
</html>

)rawliteral";

//******************************************************************************************************************
// FUNCTION PROCESSOR
//******************************************************************************************************************

String index_processor(const String& var) {

  preferences.begin("memory_cfg",false);

  if(var=="SHOW1") {

    if(wifi.radio_status=="0") {
      
      return("style=\"display:none\"");         

    } else {

      return("style=\"display:block\"");
      
    }    

  } else if(var=="RADIO1_ST") {

    if(wifi.radio_status=="1") {

      return("selected");
      
    } else {

      return(" ");
      
    }

  } else if(var=="RADIO2_ST") {

    if(wifi.radio_status=="1") {

      return(" ");
      
    } else {

      return("selected");
      
    } 

  } else if(var=="WIFI_SSID") {    

    return preferences.getString("wifi_ssid",""); 

  } else if(var=="WIFI_PASS") {

    return preferences.getString("wifi_password",""); 

  } else if(var=="ACPT_PASS") {

    return preferences.getString("ap_password","");   

  } else if(var=="WIFI_IPAD") {

    return preferences.getString("wifi_ip",""); 

  } else if(var=="WIFI_NTMA") {

    return preferences.getString("wifi_nm",""); 

  } else if(var=="WIFI_GATE") {

    return preferences.getString("wifi_gtw",""); 

  } else if(var=="WIFI_DNS") {

    return preferences.getString("wifi_dns","");     

  } else if(var=="WIFI_CPORT") {

    return preferences.getString("tcp_port",""); 

  } else if(var=="ETHE_CPORT") {

    return preferences.getString("udp_port","");   

  } else if(var=="FDIP_1") {

    return(preferences.getString("fb_ip1",""));

  } else if(var=="FDIP_2") {

    return(preferences.getString("fb_ip2",""));
       
  } else if(var=="FDIP_3") {

    return(preferences.getString("fb_ip3",""));

  } else if(var=="FDPT_1") {

    return(preferences.getString("fb_pt1",""));

  } else if(var=="FDPT_2") {

    return(preferences.getString("fb_pt2",""));

  } else if(var=="FDPT_3") {
      
    return(preferences.getString("fb_pt3",""));

  }
  
  preferences.end();

  return(" ");
  
}

//******************************************************************************************************************
