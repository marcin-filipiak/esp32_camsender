/*
 * Author: Marcin Filipiak
 * Licence: GNU-GPL
 * Board: AI-Thinker ESP32-CAM
*/

#include <Arduino.h>
#include <WiFi.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"
#include <BluetoothSerial.h>
#include <EEPROM.h>

BluetoothSerial SerialBT;

//String serverName = "192.168.1.XXX";   // REPLACE WITH SERVER API IP ADDRESS
String serverName = "api.filipiak.tech";   // OR REPLACE WITH YOUR DOMAIN NAME SERVER API
String serverPath = "/camsender/index.php?send/";  //REPLACE WITH YOUR PATH, "/" before and at the end is important!
const int serverPort = 80; //DEFAULT HTTP IS ON PORT 80

struct {
  char ssid[32] = "";  //wifi ssid
  char pass[32] = "";  //wifi pass
  char camera_id[32] = ""; //camera id for api
} data;


WiFiClient client;

// CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

const int timerInterval = 30000;    // time between each HTTP POST image
unsigned long previousMillis = 30000;   // last time image was sent


///////////////////////////////////////////
String btread(){
  String message ="";
      int e = 0;
      do {
          if (SerialBT.available()) {
            char incomingChar = SerialBT.read();
            if (incomingChar!= '\n'){
              message += String(incomingChar);
              e=0;
            }
            else e=1;
          }
      }
      while (e == 0);
  return message;
}

//czyszczenie tablicy//
void clear_array(char* a, int s){
  for(int x=0;x<s;x++){
    a[x]='\0';
  }
}

////////////////SETUP//////////////////////
void setup() {
  
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 

  EEPROM.begin(512); //EEPROM reservation for config data
  EEPROM.get(100,data); //read data config from EEPROM

  Serial.begin(115200);
  SerialBT.begin("CAMSENDER"); 
   
  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(data.ssid);
  WiFi.begin(data.ssid, data.pass);  
  int contry=0;
  while (WiFi.status() != WL_CONNECTED && contry<30) {
    Serial.print(".");
    delay(500);
    contry++;
  }
  Serial.println();
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // init with high specs to pre-allocate larger buffers
  if(psramFound()){
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 10;  //0-63 lower number means higher quality
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_CIF;
    config.jpeg_quality = 12;  //0-63 lower number means higher quality
    config.fb_count = 1;
  }
  
  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    delay(1000);
    ESP.restart();
  }

  //?? CZY ZOSTAWIC sendPhoto(); 
}
/////////////////LOOOP//////////////////////////////
void loop() {
  if (SerialBT.available()) {
    
    String menu = SerialBT.readStringUntil('\n');
    menu.trim();
    
    //help
    if (menu == "h") {
      SerialBT.println(WiFi.localIP());
      SerialBT.println("h - this help");
      SerialBT.println("r - restart");
      SerialBT.println("s - wifi scan");
      SerialBT.println("c - config");
    }
    //restart
    if (menu == "r"){
      SerialBT.println("Bye! I am restarting...");
      ESP.restart();
    }
    //wifi scan
    if (menu == "s"){
              WiFi.mode(WIFI_STA);
              WiFi.disconnect();
              delay(100);
              
              SerialBT.println("scan start - network is now disconnected!");
            
              // WiFi.scanNetworks will return the number of networks found
              int n = WiFi.scanNetworks();
              SerialBT.println("scan done");
              if (n == 0)
                SerialBT.println("");
              else
              {
                SerialBT.print(n);
                SerialBT.println(" networks found");
                for (int i = 0; i < n; ++i)
                {
                  // Print SSID and RSSI for each network found
                  SerialBT.println("");
                  SerialBT.print(i + 1);
                  SerialBT.print(": ");
                  SerialBT.print(WiFi.SSID(i));
                  SerialBT.print(" (");
                  SerialBT.print(WiFi.RSSI(i));
                  SerialBT.print(")");
                  //SerialBT.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
                  delay(10);
                }
              }
              SerialBT.println("");
    }
    //config
    if (menu == "c"){
        clear_array(data.ssid,32);
        clear_array(data.pass,32);
        clear_array(data.camera_id,32);
      
        SerialBT.println("SSID:");
        String ssid = btread();
        SerialBT.println("Saved SSID:"+ssid);
        SerialBT.println("Password:");
        String pass = btread();
        SerialBT.println("Saved password:"+pass);
        SerialBT.println("Camera ID :");
        String camera_id = btread();
        SerialBT.println("Saved camera ID:"+camera_id);

        strcpy(data.ssid, ssid.c_str());
        strcpy(data.pass, pass.c_str());
        strcpy(data.camera_id, camera_id.c_str());

        EEPROM.put(100,data);
        EEPROM.commit();
    }
  }
  

  //jesli jest siec to wysylaj zdjecia
  if (WiFi.status() == WL_CONNECTED){
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= timerInterval) {
        sendPhoto();
        previousMillis = currentMillis;
    }
  }

  
}

//////////////////SEND PHOTO//////////////////
String sendPhoto() {
  
  String getAll;
  String getBody;

  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  if(!fb) {
    Serial.println("Camera capture failed");
    delay(1000);
    ESP.restart();
  }
  
  Serial.println("Connecting to server: " + serverName);

  if (client.connect(serverName.c_str(), serverPort)) {
    Serial.println("Connection successful!");
    Serial.println("Posting to: "+ serverPath + data.camera_id);
    String head = "--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"imageFile\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--RandomNerdTutorials--\r\n";

    uint32_t imageLen = fb->len;
    uint32_t extraLen = head.length() + tail.length();
    uint32_t totalLen = imageLen + extraLen;
  
    client.println("POST " + serverPath + data.camera_id + " HTTP/1.1");
    client.println("Host: " + serverName);
    client.println("Content-Length: " + String(totalLen));
    client.println("Content-Type: multipart/form-data; boundary=RandomNerdTutorials");
    client.println();
    client.print(head);
  
    uint8_t *fbBuf = fb->buf;
    size_t fbLen = fb->len;
    for (size_t n=0; n<fbLen; n=n+1024) {
      if (n+1024 < fbLen) {
        client.write(fbBuf, 1024);
        fbBuf += 1024;
      }
      else if (fbLen%1024>0) {
        size_t remainder = fbLen%1024;
        client.write(fbBuf, remainder);
      }
    }   
    client.print(tail);
    
    esp_camera_fb_return(fb);
    
    int timoutTimer = 10000;
    long startTimer = millis();
    boolean state = false;
    
    while ((startTimer + timoutTimer) > millis()) {
      Serial.print(".");
      delay(100);      
      while (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (getAll.length()==0) { state=true; }
          getAll = "";
        }
        else if (c != '\r') { getAll += String(c); }
        if (state==true) { getBody += String(c); }
        startTimer = millis();
      }
      if (getBody.length()>0) { break; }
    }
    Serial.println();
    client.stop();
    Serial.println("Server said:");
    Serial.println(getBody);
  }
  else {
    getBody = "Connection to " + serverName +  " failed.";
    Serial.println(getBody);
  }
  return getBody;
}
