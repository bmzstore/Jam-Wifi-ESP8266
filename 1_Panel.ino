#include <Wire.h>
#include <HJS589.h>
#include <RtcDS3231.h>
RtcDS3231<TwoWire> Rtc(Wire);
#include <ArduinoJson.h>
#include <SPI.h>
#include <FS.h>

#include <fonts/ElektronMart6x8.h>
#include <fonts/ElektronMart6x16.h>
#include <fonts/Fontku.h>
#include <fonts/Mono5x7.h>
#include <fonts/SystemFont5x7.h>
#include <fonts/bmz5x7.h>
#include <fonts/bmz6x16.h>
#include <fonts/bmzSegment16.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "Webpage.h"

#define DISPLAYS_WIDE 1 
#define DISPLAYS_HIGH 1
HJS589 Disp(DISPLAYS_WIDE, DISPLAYS_HIGH);

struct ConfigDisp{
  int cerah;
  };

// HJS589 P10 FUNGSI TAMBAHAN UNTUK NODEMCU ESP8266

void ICACHE_RAM_ATTR refresh() { 
  
  Disp.refresh();
  timer0_write(ESP.getCycleCount() + 80000);  

}

void Disp_init() {
  
  Disp.start();
  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(refresh);
  timer0_write(ESP.getCycleCount() + 80000);
  interrupts();
  Disp.clear();
}

//SETUP RTC
//year, month, date, hour, min, sec and week-day(Senin 0 sampai Ahad 6)
//DateTime dt(2019, 11, 7, 21, 15, 0, 3);
RtcDateTime now;
char weekDay[][7] = {"MINGGU", "SENIN", "SELASA", "RABU", "KAMIS", "JUM'AT", "SABTU", "MINGGU"}; // array hari, dihitung mulai dari senin, hari senin angka nya =0,
char monthYear[][10] = { "DESEMBER", "JANUARI", "PEBRUARI", "MARET", "APRIL", "MEI", "JUNI", "JULI", "AGUSTUS", "SEPTEMBER", "OKTOBER", "NOPEMBER", "DESEMBER" };

char jam[9];
byte tmpinfo;
static uint32_t pM,pMJam,cM;

// WIFI
// LED Internal
uint8_t pin_led = 2;

//WEB Server
ESP8266WebServer server(80);

// Sebagai Station 
 const char* wifissid = "bmzstore"; 
 const char* wifipassword = "12345678";

// Sebagai AP
const char* ssid = "WifiClock"; //kalau gagal konek
const char* password = "123456789";

IPAddress local_ip(10, 10, 10, 10);
IPAddress gateway(192, 168, 20, 20);
IPAddress netmask(255, 255, 255, 0);

void wifiConnect() {

  WiFi.softAPdisconnect(true);
  WiFi.disconnect();
  delay(1000);

  Serial.println("Wifi Sation Mode");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);
  
  unsigned long startTime = millis();
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    digitalWrite(pin_led, !digitalRead(pin_led));
    if (millis() - startTime > 3000) break;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(pin_led, HIGH);
    
  } else {
    
    Serial.println("Wifi AP Mode");
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_ip, gateway, netmask);
    WiFi.softAP(ssid, password);
    digitalWrite(pin_led, LOW);
  }

  //Serial.println("");
  WiFi.printDiag(Serial);
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

}

const char *fileconfigdisp = "/configdisp.json";
ConfigDisp configdisp;

//-------------------------------------------
//SETUP

 void setup() {
  Serial.begin(115200);
 
  //RTC D3231

  int rtn = I2C_ClearBus(); // clear the I2C bus first before calling Wire.begin()
  if (rtn != 0) {
    Serial.println(F("I2C bus error. Could not clear"));
    if (rtn == 1) {
      Serial.println(F("SCL clock line held low"));
    } else if (rtn == 2) {
      Serial.println(F("SCL clock line held low by slave clock stretch"));
    } else if (rtn == 3) {
      Serial.println(F("SDA data line held low"));
    }
  } else { // bus clear, re-enable Wire, now can start Wire Arduino master
    Wire.begin();
  }
  
  Rtc.Begin();

  if (!Rtc.GetIsRunning()) {
    
    Rtc.SetIsRunning(true);
    
  }
  
  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone); 
  
 //Rtc.SetDateTime(RtcDateTime(2019,11,26,21,39,0)); // Setting Jam Manual
  
  Serial.println("Setup RTC selesai");

  pinMode(pin_led, OUTPUT);
  
  SPIFFS.begin();

  loadDispConfig(fileconfigdisp, configdisp);
  
  LoadDataAwal();

  wifiConnect();
 server.on("/", []() {
    server.send_P(200, "text/html", setwaktu);

    // Kalau ada perubahan tanggal
    if (server.hasArg("date")) {
      uint16_t jam;
      uint8_t menit;
      uint8_t detik;      
      String sd=server.arg("date");
      String lastSd;
      
      jam = ((sd[0]-'0')*1000)+((sd[1]-'0')*100)+((sd[2]-'0')*10)+(sd[3]-'0');
      menit = ((sd[5]-'0')*10)+(sd[6]-'0');
      detik = ((sd[8]-'0')*10)+(sd[9]-'0');
      
      if (sd != lastSd){
        RtcDateTime now = Rtc.GetDateTime();
        uint8_t hour = now.Hour();
        uint8_t minute = now.Minute();
        Rtc.SetDateTime(RtcDateTime(jam, menit, detik, hour, minute, 0));
        lastSd=sd;
      }
  
     server.send ( 404 ,"text", message );
    }
      // Kalau ada perubahaan jam
    if (server.hasArg("time")) {
      
      String st=server.arg("time");
      String lastSt;
      uint8_t jam = ((st[0]-'0')*10)+(st[1]-'0');
      uint8_t menit = ((st[3]-'0')*10)+(st[4]-'0');
      
      if (st != lastSt){
         RtcDateTime now = Rtc.GetDateTime();
         uint16_t year = now.Year();
         uint8_t month = now.Month();
         uint8_t day = now.Day();
         Rtc.SetDateTime(RtcDateTime(year, month, day, jam, menit, 0));
         lastSt=st;}
      server.send ( 404 ,"text", message );
  
    }
  });
  
 server.on("/setdisplay", []() {
    server.send_P(200, "text/html", setdisplay);
  });  

  server.on("/settingdisp", HTTP_POST, handleSettingDispUpdate);

  //server.on ( "/xml", handleXML) ;  

 
  server.begin();
  Serial.println("HTTP server started");
    
  //DMD
  Disp_init();
  Disp.setBrightness(configdisp.cerah);

 }
 
//--------------------------------------------------------
// LOOP

void loop() {   
  server.handleClient();
  
  switch(tmpinfo){
    case 0:
    JamBesar();
    break;

    case 1:
    JamKecil();
    TampilHariTanggal();
    break;

    case 2:
    tmpinfo=0;
    Disp.clear();
    break;
  }
  
 // JamBesar();
 }

//---------------------------------------------------------
//JAM BESAR

void JamBesar(){
  RtcDateTime now = Rtc.GetDateTime();
  uint32_t cM = millis();

  char jam[3];
  char menit[3];
  char detik[3];
    
  sprintf(jam,"%02d", now.Hour());
  sprintf(menit,"%02d", now.Minute());
  sprintf(detik,"%02d", now.Second());

  Disp.setFont(bmz6x16);
  Disp.drawText(1,0,jam);

  Disp.drawRect(16,4,17,6,1,1);
  Disp.drawRect(16,9,17,11,1,1);

  Disp.setFont(SystemFont5x7);
  Disp.drawText(20,0,menit);
  Disp.drawText(20,9,detik);
  
   if (cM -pM >= 15000) {   // LAMANYA TAMPIL
   pM = cM;
   tmpinfo++;
   Disp.drawRect(0,0,64,16,0,0);
   }
  }
  
//-----------------------------------------------------
//TAMPILAN JAM KECIL

void JamKecil() {
  uint32_t cM = millis();

  RtcDateTime now = Rtc.GetDateTime();

  if (cM - pMJam >= 1000) {
    pMJam = cM ;

   //JAM
   sprintf(jam,"%02d:%02d", now.Hour(), now.Minute());
   Disp.setFont(SystemFont5x7);
   Disp.drawText(1,0, jam);
     }
}

//----------------------------------------------------
//TAMPILAN HARI DAN TANGGAL

void TampilHariTanggal(){
  RtcDateTime now = Rtc.GetDateTime ();
  RtcTemperature temp = Rtc.GetTemperature();
  
  int celcius = temp.AsFloatDegC();
  int koreksisuhu = 2;
  uint32_t cM = millis();
  static uint32_t kecepatan = 70;
  static uint32_t x;
  char tanggal[21]; // SABTU 10 NOP 2019

  sprintf(tanggal, "%s %02d %s %02d SUHU %02d CELCIUS", weekDay[now.DayOfWeek()], now.Day(), monthYear[now.Month()], now.Year(), celcius - koreksisuhu);
 
  Disp.setFont(SystemFont5x7);
  int width = Disp.width();
  int fullScroll = Disp.textWidth(tanggal) + width;

   if(cM - pM > kecepatan){
    pM = cM ;
    if(x < fullScroll) {
      ++x;
    } else {
      x = 0 ;
      tmpinfo++;
    }
    Disp.drawText(width - x, 8, tanggal);
   }
  
}

//----------------------------------------------------------------

void loadDispConfig(const char *fileconfigdisp, ConfigDisp &configdisp) {

  File configFileDisp = SPIFFS.open(fileconfigdisp, "r");

  if (!configFileDisp) {
    Serial.println("Gagal membuka fileconfigdisp untuk dibaca");
    return;
  }

  size_t size = configFileDisp.size();
  std::unique_ptr<char[]> buf(new char[size]);
  configFileDisp.readBytes(buf.get(), size);

  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, buf.get());

  if (error) {
    Serial.println("Gagal parse fileconfigdisp");
    return;
  }
  
  configdisp.cerah = doc["cerah"];

  configFileDisp.close();

}

void handleSettingDispUpdate() {

  timer0_detachInterrupt();
  
  String datadisp = server.arg("plain");
  
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, datadisp);

  File configFileDisp = SPIFFS.open(fileconfigdisp, "w");
  
  if (!configFileDisp) {
    Serial.println("Gagal membuka Display configFile untuk ditulis");
    return;
  }
  
  serializeJson(doc, configFileDisp);

  if (error) {
    
    Serial.print(F("deserializeJson() gagal kode sebagai berikut: "));
    Serial.println(error.c_str());
    return;
    
  } else {
    
    configFileDisp.close();
    Serial.println("Berhasil mengubah configFileDisp");

    server.send(200, "application/json", "{\"status\":\"ok\"}");

    loadDispConfig(fileconfigdisp, configdisp);
    
    delay(500);
    Disp_init();

    Disp.setBrightness(configdisp.cerah);
  
  }  

}

//Load Data Awal
//------------------------------------------------------------------------
void LoadDataAwal(){
  if (configdisp.cerah == 0) {
    configdisp.cerah = 100;    
  }
}

//----------------------------------------------------------------------
// I2C_ClearBus menghindari gagal baca RTC (nilai 00 atau 165)

int I2C_ClearBus() {
  
#if defined(TWCR) && defined(TWEN)
  TWCR &= ~(_BV(TWEN)); //Disable the Atmel 2-Wire interface so we can control the SDA and SCL pins directly
#endif

  pinMode(SDA, INPUT_PULLUP); // Make SDA (data) and SCL (clock) pins Inputs with pullup.
  pinMode(SCL, INPUT_PULLUP);

  delay(2500);  // Wait 2.5 secs. This is strictly only necessary on the first power
  // up of the DS3231 module to allow it to initialize properly,
  // but is also assists in reliable programming of FioV3 boards as it gives the
  // IDE a chance to start uploaded the program
  // before existing sketch confuses the IDE by sending Serial data.

  boolean SCL_LOW = (digitalRead(SCL) == LOW); // Check is SCL is Low.
  if (SCL_LOW) { //If it is held low Arduno cannot become the I2C master. 
    return 1; //I2C bus error. Could not clear SCL clock line held low
  }

  boolean SDA_LOW = (digitalRead(SDA) == LOW);  // vi. Check SDA input.
  int clockCount = 20; // > 2x9 clock

  while (SDA_LOW && (clockCount > 0)) { //  vii. If SDA is Low,
    clockCount--;
  // Note: I2C bus is open collector so do NOT drive SCL or SDA high.
    pinMode(SCL, INPUT); // release SCL pullup so that when made output it will be LOW
    pinMode(SCL, OUTPUT); // then clock SCL Low
    delayMicroseconds(10); //  for >5uS
    pinMode(SCL, INPUT); // release SCL LOW
    pinMode(SCL, INPUT_PULLUP); // turn on pullup resistors again
    // do not force high as slave may be holding it low for clock stretching.
    delayMicroseconds(10); //  for >5uS
    // The >5uS is so that even the slowest I2C devices are handled.
    SCL_LOW = (digitalRead(SCL) == LOW); // Check if SCL is Low.
    int counter = 20;
    while (SCL_LOW && (counter > 0)) {  //  loop waiting for SCL to become High only wait 2sec.
      counter--;
      delay(100);
      SCL_LOW = (digitalRead(SCL) == LOW);
    }
    if (SCL_LOW) { // still low after 2 sec error
      return 2; // I2C bus error. Could not clear. SCL clock line held low by slave clock stretch for >2sec
    }
    SDA_LOW = (digitalRead(SDA) == LOW); //   and check SDA input again and loop
  }
  if (SDA_LOW) { // still low
    return 3; // I2C bus error. Could not clear. SDA data line held low
  }

  // else pull SDA line low for Start or Repeated Start
  pinMode(SDA, INPUT); // remove pullup.
  pinMode(SDA, OUTPUT);  // and then make it LOW i.e. send an I2C Start or Repeated start control.
  // When there is only one I2C master a Start or Repeat Start has the same function as a Stop and clears the bus.
  /// A Repeat Start is a Start occurring after a Start with no intervening Stop.
  delayMicroseconds(10); // wait >5uS
  pinMode(SDA, INPUT); // remove output low
  pinMode(SDA, INPUT_PULLUP); // and make SDA high i.e. send I2C STOP control.
  delayMicroseconds(10); // x. wait >5uS
  pinMode(SDA, INPUT); // and reset pins as tri-state inputs which is the default state on reset
  pinMode(SCL, INPUT);
  return 0; // all ok
}
