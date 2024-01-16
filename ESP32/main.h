const char MainPage[] PROGMEM = R"=====(


<!DOCTYPE html>
<html>
  <head>
    <title>SMART HOME</title>
    <style>
      body{text-align:center;}
      h1{color:#003399;}
      a {text-decoration: none;color:#FFFFFF;}
      .bt_on {height:50px; width:100px; margin:10px 0; background-color:#FF6600; border-radius:5px;}
      .bt_off {height:50px; width:100px; margin:10px 0; background-color:#FF6600; border-radius:5px;}
    </style>
    <meta charset = "UTF-8">
  </head>
  
  <body>
    <h1>MÔ HÌNH NHÀ THÔNG MINH</h1>
    <div>
      Nhiệt độ: <span id="nhietdo">0</span> C<br>
      Độ ẩm   : <span id="doam">0</span> %<br>
    </div>
    <div>Cảnh báo khi gas: <span id="gas">OK</span></div>
    <div>Chế độ tự động: <span id="autox">ON</span></div><br><br>

    <div>Trạng thái QUẠT <b><span id="quat">ON</span></b> </div>
    <div>
      <button class="bt_on" onclick="getdata('onQuat')">ON</button>
      <button class="bt_off" onclick="getdata('offQuat')">OFF</button>
    </div>
    <div>Trạng thái SƯỞI: <b><span id="suoi">ON</span></b> </div>
    <div>
      <button class="bt_on" onclick="getdata('onSuoi')">ON</button>
      <button class="bt_off" onclick="getdata('offSuoi')">OFF</button>
    </div>



    <!-- JS -->
    <script>
       function getdata(url){
        var xhttp1 = new XMLHttpRequest();
        xhttp1.open("GET","/"+url,true);
        xhttp1.send();
      }
        setInterval(function(){            
            getData();
        },1000);

//         function getnhietdo(){
//          var xhttp1 = new XMLHttpRequest();
//          xhttp1.open("GET","docnhietdo",true);
//          xhttp1.onreadystatechange = function(){
//            if(this.readyState == 4 && this.status == 200){
//              document.getElementById("nhietdo").innerHTML=this.responseText;
//            }
//          }
//          xhttp1.send();
//        }
  
        var xhttp = new XMLHttpRequest();
        function getData(){
            xhttp.open("GET","getdataJS",true);
            xhttp.onreadystatechange =process_json2;
            xhttp.send();
        }
        function process_json2(){
            if(xhttp.readyState == 4 && xhttp.status == 200){
                var data_JSON = xhttp.responseText;
                var Obj = JSON.parse(data_JSON);
                document.getElementById("nhietdo").innerHTML = Obj.nhietdo;
                document.getElementById("doam").innerHTML = Obj.doam;
                document.getElementById("gas").innerHTML = Obj.gas;
                document.getElementById("autox").innerHTML = Obj.auto1;
                document.getElementById("quat").innerHTML = Obj.quat;
                document.getElementById("suoi").innerHTML = Obj.suoi;                             
            }
        }

    </script>

  </body>
</html>



)=====";
