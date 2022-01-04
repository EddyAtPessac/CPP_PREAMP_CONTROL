#include <Arduino.h>
#include "heltec.h"
#include <Adafruit_neopixel.h>
#include "PinDefinition.h"
#include "Encoder.h"
#include <IRremote.h>
#include "PrintUtil.h"
#include "PotRing.h"
#include "LogicPotentiometer.h"

#define OTA_UPLOAD 1

#define OTA_PRINT 1

#if OTA_UPLOAD
// Special for OTA
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "PrintUtil.h"

const char* ssid = "FREE_DOM";
const char* password = "YOUR_PSW";
#endif
#if OTA_PRINT
#include <RemoteDebug.h>
RemoteDebug Debug;
#endif

IRrecv ir_recv(PIN_IR_RECEVER);
decode_results ir_data;



// Exemple: https://byfeel.info/bibliotheque-neopixel-avec-un-esp8266/
// Nombre de led
#define LED_COUNT  24
// NeoPixel luminosite, 0 (min) to 255 (max)
#define BRIGHTNESS 5

PotRing potRing(LED_COUNT, LED_PIN);

LogicPotentiometer *potVol; // Attention, ici c'est un pointeur ...
LogicPotentiometer *potBal; 
LogicPotentiometer *potBas; 
LogicPotentiometer *potTre; 

#include "Adafruit_NeoPixel.h"

void setup_OTA() {

  WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("Connection Failed! Rebooting...");
      delay(5000);
      ESP.restart();
    }
// Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  Debug.begin("ESP32"); 
  Debug.print("Start ESP OVA");

}

void setup() {
  // Check fToStr()
  double level = 1.2;
  double inc = 0.6;

  // put your setup code here, to run once:
  Heltec.begin(false /* Display Ena*/,  false /* Lora ena */, true /* serial ena */); 

  setup_OTA();
 
  Serial.println("Starting Encoder  ...");
  InitialiseEncoder(PIN_ENCODER_A, PIN_ENCODER_B, PIN_ENCODER_PUSH, 
                    ChangeSelectedPotLevel, SelectNextPotentiometer );

  Serial.println("Starting potentiometer ...");
  sprintf(printStr,"Pot %s add %s inc level= %s", "test", fToStr(fbuf1, inc), fToStr(fbuf2, level)); 
  Serial.println(printStr);
  print_pool(printStr);
  
  potVol = new LogicPotentiometer(PIN_PWM_VOL, RC_COLOR_RED, "Vol", &potRing, 0.5);
  potBal = new LogicPotentiometer(PIN_PWM_BAL, RC_COLOR_MAGENTA, "Bal", &potRing, 0.5);
  potBas = new LogicPotentiometer(PIN_PWM_TRE, RC_COLOR_BLUE, "Bas", &potRing, 0.5);
  potTre = new LogicPotentiometer(PIN_PWM_BAS, RC_COLOR_GREEN, "Tre", &potRing, 0.5);

  potVol->AttachIRCode(0x08F7, 0x28d7);
  potBal->AttachIRCode(0x8877, 0x6897);
  potBas->AttachIRCode(0x10EF, 0x30CF);
  potTre->AttachIRCode(0x50AF, 0x708F);
  potVol->ChangeLevel(0.0); // Select PotVol and initialise ring display for potVol
  ir_recv.enableIRIn();
}

int compteur =0;
  
void loop() {
  // put your main code here, to run repeatedly:
  static int i=0;
  ArduinoOTA.handle();
  Debug.handle();

  delay(5);
  
  potRing.UpdateRing();   // Change ring indicator if necessary 

  print_loop_handle();    // Display the pushed messages

  CheckPotLoop();           // Update potentiometer

  if (ir_recv.decode(&ir_data)) {
    // Serial.println(ir_data.value, HEX);
    CheckPotentiometerIRData(ir_data.value, false); // The repeat flag doesnt works.
    ir_recv.resume();
  }

}

