//******************************************************************************************************************
// Created by Pedro Braida Neto
// 01/31/2022 All Rights Reserved
//******************************************************************************************************************

//******************************************************************************************************************
// RESET.HTML PAGE
//******************************************************************************************************************
const char reset_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
  <html>
  <head>
    <title>DBX-Mio1012</title>
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
      .leftcolumn {
        text-align: center;   
        float: left;
        width: 100%;
      }  
      .bcol-r {
        text-align: center;
        float: center;
        margin-top: 2px;
        font-size: 20px;
      }          
      input[type=submit] {
        background-color: black;
        color: white;
        font-size: 25px;
        padding: 10px 30px;
        border: none;
        border-radius: 8px;
        cursor: pointer;
        margin: 8px 5px;
        float: center;
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);
      } 
      input[type=submit]:active {
        background-color: #4682B4;
        color: white;
        box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0), 0 6px 20px 0 rgba(0, 0, 0, 0);
        transform: translateY(4px);        
      }
      img {
        max-width: 100%;
        height: auto;
      } 
      @media screen and (max-height: 1200pxpx) {
        .sidebar {padding-top: 15px;}
        .sidebar a {font-size: 18px;}
      }       
      @media screen and (max-width: 800px) {
        .bcol-r {
          width: 100%;
          height: auto;
          text-align: center;
          float: center;
        }
      }        
      @media screen and (max-width: 800px) {
        .leftcolumn {   
          width: 100%;
          padding: 0;
        }
      }                    
      @media screen and (max-height: 800px) {
        .sidebar {padding-top: 15px;}
        .sidebar a {font-size: 18px;}
      }
      @media screen and (max-width: 800px) {
        .texto_1 {
          font-size: 15px;
        }
      }      
      @media screen and (max-width: 800px) {
        .logo {
          float: center;
          width: 100%;
        }
        .logo_1 {
          float: center;
          width: 100%;
        }
      }                                                      
    </style>
  </head>
  <body>
    <div id="mySidebar" class="sidebar">
      <a href="javascript:void(0)" class="closebtn" onclick="closeNav()">x</a>
      <a href="/">Status</a>           
      <a href="/stools">Tools</a>
    </div>
    <div id="main">
      <div class="header">
        <div class="logo">
          <a href="/">
          <img src="logo" width="213" height="90">
          </a>
        </div>  
        <div class="texto_1">
          DBX-Mio1012
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
          Reset $ACCO_LOC$
        </div>
      </div> 
      <div class="row">  
        <div class="leftcolumn"> 
          <form action="/apreset.php">
            <h2>It is necessary to reset the controller for the changes to take effect !!!</h2>
            <h2>This operation takes approximately 30 seconds, please press the reset button and wait...</h2>
            <div class="bcol-r">
              <input type="submit" value="Reset">
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
// FUNCTION PROCESSOR_UPDATE
//******************************************************************************************************************

String reset_processor(const String& var) {

  return "";
  
}

//******************************************************************************************************************
