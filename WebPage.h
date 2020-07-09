String message, XML;

char setwaktu[] PROGMEM = R"=====(

<!DOCTYPE HTML>
<html>
<body bgcolor="#90EE90">
<meta name='viewport' content='width=device-width, initial-scale=1'>

<head>
<title>Wifi Clock ESP8266</title>
<style> body { width: 97% ; text-align: center; } input, select, button, textarea { max-width: 100% ; margin: 5px; padding: 5px; border-radius: 7px; } meter, progress, output { max-width: 100% ; } h1 { color: grey; } .note { color: #555; font-size:1em;} .info { background-color: #eee; border-radius: 3px; padding-top: 17px; margin: 5px;} .tombol { background-color: #ccc; margin: 20px; padding: 10px; border-radius: 10px; text-decoration: none;}</style>
</head>

<body onload='process()'>

<h1>SETTING JAM DAN TANGGAL</h1>
<table width='100%'>
  <tr>
    <td>
      <form>
        <h4>Tanggal</h4>  
        <input type='date' name='date' min='2020-01-01'><br><br>
        <input type='submit' value='Ubah Tanggal'> 
      </form>  
    </td>
    <td>
      <form>  
        <h4>Jam</h4>
        <input type='TIME' name='time'><br><br>
        <input type='submit' value='Ubah Jam'> 
      </form>
    </td>
  </tr>
</table>
<br><br>
<h5><a class ='tombol' href="/setdisplay">Brightness</a></h5>
<br><br>
<a class ='tombol' href="http://dickybmz.com/">Whatsapp (0813-9154-3454)</a><br><br><br>
<a class ='tombol' href="https://www.bukalapak.com/u/bmzstore">Bukalapak</a><span> </span>
<a class ='tombol' href="https://www.tokopedia.com/dickybmz">Tokopedia</a><br>
</body>
</html>
<script>

var xmlHttp=createXmlHttpObject();

function createXmlHttpObject(){
 if(window.XMLHttpRequest){
    xmlHttp=new XMLHttpRequest();
 }else{
    xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');// code for IE6, IE5
 }
 return xmlHttp;
}

function process(){
 if(xmlHttp.readyState==0 || xmlHttp.readyState==4){
   xmlHttp.open('PUT','xml',true);
   xmlHttp.onreadystatechange=handleServerResponse;
   xmlHttp.send(null);
 }
 setTimeout('process()',1000);
}

function handleServerResponse(){
 if(xmlHttp.readyState==4 && xmlHttp.status==200){
   xmlResponse=xmlHttp.responseXML;

   xmldoc = xmlResponse.getElementsByTagName('rYear');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('year').innerHTML=message;

   xmldoc = xmlResponse.getElementsByTagName('rMonth');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('month').innerHTML=message;

   xmldoc = xmlResponse.getElementsByTagName('rDay');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('day').innerHTML=message;

   xmldoc = xmlResponse.getElementsByTagName('rHour');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('hour').innerHTML=message;

   xmldoc = xmlResponse.getElementsByTagName('rMinute');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('minute').innerHTML=message;

   xmldoc = xmlResponse.getElementsByTagName('rSecond');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('second').innerHTML=message;
 }
}
</script>
</html>
)=====";


char setdisplay[] PROGMEM = R"=====(

<!DOCTYPE HTML>
<html>
<body bgcolor="#90EE90">
<meta name='viewport' content='width=device-width, initial-scale=1'>
<head>
  <title>Jam-Wifi</title>
  <style> body { width: 97% ; text-align: center; } input, select, button, textarea { max-width: 100% ; margin: 5px; padding: 5px; border-radius: 7px; } meter, progress, output { max-width: 100% ; } h1 { color: grey; } .note { color: #555; font-size:1em;} .info { background-color: #eee; border-radius: 3px; padding-top: 17px; margin: 5px;} .tombol { background-color: #ccc; margin: 20px; padding: 10px; border-radius: 10px; text-decoration: none;}</style>
</head>
<body onload='process()'>
<h1>SETTING KECERAHAN</h1>
<form id='data_form' name='frmText'>
  <label>Redup<input id="cerah" type='range' name='Cerah' min='18' max='255'>Cerah
  <br>
</form>
<br>
<button onClick="setDisp()"> Simpan </button><span> </span>
<br><br>
<h5><a class ='tombol' href="/">Kembali</a></h5>
<br><br>
<a class ='tombol' href="http://dickybmz.com/">Whatsapp (0813-9154-3454)</a><br><br><br>
<a class ='tombol' href="https://www.bukalapak.com/u/bmzstore">Bukalapak</a><span> </span>
<a class ='tombol' href="https://www.tokopedia.com/dickybmz">Tokopedia</a><br>
</body>

<script>

function setDisp() {

  console.log("tombol telah ditekan!");
  
  var cerah = document.getElementById("cerah").value;
  var datadisp = {cerah:cerah};

  var xhr = new XMLHttpRequest();
  var url = "/settingdisp";

  xhr.onreadystatechange = function() {
    if(this.onreadyState == 4  && this.status == 200) {
      console.log(xhr.responseText);
    }
  };
  xhr.open("POST", url, true);
  xhr.send(JSON.stringify(datadisp));
  
}

var xmlHttp=createXmlHttpObject();

function createXmlHttpObject(){
 if(window.XMLHttpRequest){
    xmlHttp=new XMLHttpRequest();
 }else{
    xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');// code for IE6, IE5
 }
 return xmlHttp;
}

function process(){
 if(xmlHttp.readyState==0 || xmlHttp.readyState==4){
   xmlHttp.open('PUT','xml',true);
   xmlHttp.onreadystatechange=handleServerResponse;
   xmlHttp.send(null);
 }
}

function handleServerResponse(){
 if(xmlHttp.readyState==4 && xmlHttp.status==200){
   xmlResponse=xmlHttp.responseXML;

   xmldoc = xmlResponse.getElementsByTagName('rCerah');
   message = xmldoc[0].firstChild.nodeValue;
   document.getElementById('cerah').value=message;

 }
}
</script>
</html>

)=====";
