/*
 * Author: Marcin Filipiak
 * Licence: GNU-GPL
 * Board: AI-Thinker ESP32-CAM
 * 
 * You can config this camera by Android APP:
 * https://github.com/marcin-filipiak/IoT_Configurator
 * 
*/

#include <Arduino.h>
#include <WiFi.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"
#include <BluetoothSerial.h>
#include <EEPROM.h>

#include <DeviceConfigJSON.h>
DeviceConfigJSON DeviceConfigJSON("Config","Camsender config");

BluetoothSerial SerialBT;

const int serverPort = 80; //DEFAULT HTTP IS ON PORT 80

bool confsend = false;

struct {
  char ssid[32] = "";  //wifi ssid
  char pass[32] = "";  //wifi pass
  char camera_id[32] = ""; //camera id for api
  char serverName[32] = "";
  char serverPath[50] = "";
  int timerInterval = 30000;
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

//const int timerInterval = 30000;    // time between each HTTP POST image
unsigned long previousMillis = 30000;   // last time image was sent


///////////////////////////////////////////

///led blink
void blink(){
   digitalWrite(4, HIGH); 
   delay(500);
   digitalWrite(4, LOW); 
}


void clear_array(char* a, int s){
  for(int x=0;x<s;x++){
    a[x]='\0';
  }
}


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
  
  Serial.println("Connecting to server: " + String(data.serverName));

  if (client.connect(data.serverName, serverPort)) {
    Serial.println("Connection successful!");
    Serial.println("Posting to: "+ String(data.serverPath) + data.camera_id);
    String head = "--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"imageFile\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--RandomNerdTutorials--\r\n";

    uint32_t imageLen = fb->len;
    uint32_t extraLen = head.length() + tail.length();
    uint32_t totalLen = imageLen + extraLen;
  
    client.println("POST " + String(data.serverPath) + String(data.camera_id) + " HTTP/1.1");
    client.println("Host: " + String(data.serverName));
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
    getBody = "Connection to " + String(data.serverName) +  " failed.";
    Serial.println(getBody);
  }
  return getBody;
}

////////////////SETUP//////////////////////
void setup() {
  
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); 

  pinMode(4, OUTPUT);

  EEPROM.begin(512); //EEPROM reservation for config data
  EEPROM.get(100,data); //read data config from EEPROM

  //uart for debuging
  Serial.begin(115200);

  //bluetooth for config
  if (data.camera_id != ""){
    SerialBT.begin(data.camera_id); 
  }
  else {
    SerialBT.begin("CAMSENDER"); 
  }
   
  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(data.ssid);
  WiFi.begin(data.ssid, data.pass);  
  int contry=0;
  //try connect to wifi
  while (WiFi.status() != WL_CONNECTED && contry<30) {
    Serial.print(".");
    delay(500);
    contry++;
  }

  blink();
  
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

  //?? sendPhoto(); 
}
/////////////////LOOOP//////////////////////////////
void loop() {

  //if incoming JSON configuration from Bluetooth
  if (SerialBT.available()) {

        clear_array(data.ssid,32);
        clear_array(data.pass,32);
        clear_array(data.camera_id,32);
        clear_array(data.serverName,32);
        clear_array(data.serverPath,50);
        
        String incoming = SerialBT.readString();
        String camera_id = DeviceConfigJSON.getValue(incoming, "camera_id");
        String ssid = DeviceConfigJSON.getValue(incoming, "ssid");
        String pass = DeviceConfigJSON.getValue(incoming, "pass");
        String svp = DeviceConfigJSON.getValue(incoming, "serverPath");
        String svn = DeviceConfigJSON.getValue(incoming, "serverName");
        String tint = DeviceConfigJSON.getValue(incoming, "timerInterval");

        strcpy(data.ssid, ssid.c_str());
        strcpy(data.pass, pass.c_str());
        strcpy(data.camera_id, camera_id.c_str());
        strcpy(data.serverPath, svp.c_str());
        strcpy(data.serverName, svn.c_str());
        data.timerInterval = tint.toInt();

        EEPROM.put(100,data);
        EEPROM.commit();
        
  }

  // If device is connected by bluetooth - send config JSON after 1000ms
  if (SerialBT.connected() && !confsend) {
        digitalWrite(4, HIGH); 
        delay(1000);
        digitalWrite(4, LOW); 

        //DeviceConfigJSON.label("IP",WiFi.localIP()));
        DeviceConfigJSON.text("Camera id","camera_id",data.camera_id);
        DeviceConfigJSON.text("SSID","ssid",data.ssid);
        DeviceConfigJSON.password("password","pass",data.pass);

        DeviceConfigJSON.text("Timer","timerInterval",String(data.timerInterval));

        DeviceConfigJSON.text("Server name","serverName",data.serverName);
        DeviceConfigJSON.text("Server path","serverPath",data.serverPath);

        // string with generated json
        String jsonString = DeviceConfigJSON.getJSON();

        // clear json for new configuration
        DeviceConfigJSON.clear();

        //now we send JSON
        SerialBT.println(jsonString);

        //set true, because we send only one time
        confsend = true; 
  }  
  

  //if connected to wifi send photo
  if (WiFi.status() == WL_CONNECTED){
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= data.timerInterval) {
        sendPhoto();
        previousMillis = currentMillis;
    }
  }

  
}
