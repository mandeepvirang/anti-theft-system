#include <HTTPClient.h>
#include <FS.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include "esp_camera.h"
#include "SPI.h"
#include "driver/rtc_io.h"
#include "ESP32_MailClient.h"
#include "ThingSpeak.h"

const char* ssid = "my_wifi";
const char* password = "password";

const char* iftttURL = "https://maker.ifttt.com/trigger/ESP_MOTION/with/key/kfmK343pwtR4eiRFiv4t_fSxHmzLvXa6x1xkCEN9IVz4z";
int pir = 15;  // Data pin for pir
int pirVal = 0;

unsigned long myChannelNumber =  1217490;  
const char * myWriteAPIKey = "ZK4W3FY709CSXIN3"; 

#define emailSenderAccount    "tempmail12344321@gmail.com"
#define emailSenderPassword   "12344321"
#define smtpServer            "smtp.gmail.com"
#define smtpServerPort         465
#define emailSubject          "Intruder detected, check the image immediately"
#define emailRecipient        "mandeepvirang1121@gmail.com"

#define CAMERA_MODEL_AI_THINKER

#include "cameraPins.h"


SMTPData smtpData;


#define FILE_PHOTO "/image.jpg"

WiFiClient client;

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  
  Serial.begin(9600);
  pinMode(pir, INPUT);
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi.....");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi successfully!!");

  ThingSpeak.begin(client);
  
  if (!SPIFFS.begin(true)) {
    Serial.println("Unable to mount SPIFFS");
    ESP.restart();
  }
  else {
    delay(500);
    Serial.println("SPIFFS mounted successfully!!");
  }


   
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
  
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }


  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.println("Unable to initialize camera!");
    return;
  }
}

void loop(){
  if(WiFi.status() != WL_CONNECTED){connect_to_wifi();}
  pirVal = digitalRead(pir);
  delay(1000);
  if (WiFi.status() == WL_CONNECTED){
    ThingSpeak.writeField(myChannelNumber, 1, pirVal, myWriteAPIKey);
  }
  if(pir == HIGH){
    if(WiFi.status() == WL_CONNECTED){
      
      HTTPClient http;                    
      http.begin(iftttURL);     
      int httpCode = http.GET();
      Serial.println("Done");
      if (httpCode > 0) {
        Serial.println(http.getString());

      } 
      http.end();
    
      CaptureImage();
      sendImage();
      
    }
    delay(8000);
  }
  delay(2000);
}

void connect_to_wifi(){
  while (WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, password);
      Serial.print(".");
      delay(5000);     
  }
  return;
}

// Check if photo capture was successful
bool checkStatus( fs::FS &fs ) {
  File f_pic = fs.open( FILE_PHOTO );
  unsigned int pic_sz = f_pic.size();
  return ( pic_sz > 100 );
}

// Capture Photo and Save it to SPIFFS
void CaptureImage( void ) {
  camera_fb_t * fb = NULL; 
  bool ok = 0;
  do {
    Serial.println("Capturing Image...");
    fb = esp_camera_fb_get();
    if (!fb) {immediately
      Serial.println("Couldn't capture Image");
      return;
    }
    File file = SPIFFS.open(FILE_PHOTO, FILE_WRITE);
    file.write(fb->buf, fb->len);
    Serial.print("Image saved.");
    file.close();
    esp_camera_fb_return(fb);
    ok = checkStatus(SPIFFS);
  } while ( !ok );
  
}

//send image via email
void sendImage( void ) {
  Serial.println("Sending email...");
  smtpData.setLogin(smtpServer, smtpServerPort, emailSenderAccount, emailSenderPassword);
  smtpData.setSender("ESP32-CAM", emailSenderAccount);
  
  smtpData.setPriority("High");
  smtpData.setSubject(emailSubject);
  smtpData.setMessage("<h2>Photo captured of the intruder is attached, take necessary action immediately</h2>", true);
  smtpData.addRecipient(emailRecipient);
  smtpData.addAttachFile(FILE_PHOTO, "image/jpg");
  smtpData.setFileStorageType(MailClientStorageType::SPIFFS);

  if (!MailClient.sendMail(smtpData))
    Serial.println("Error sending Email, " + MailClient.smtpErrorReason());
  smtpData.empty();
}

