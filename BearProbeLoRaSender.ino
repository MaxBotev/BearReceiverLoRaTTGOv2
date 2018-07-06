/*
Deep Sleep with External Wake Up
=====================================

*/
#include <SPI.h>
#include "LoRa.h"
#include <SSD1306.h>
#include<Arduino.h>

//OLED pins to ESP32 GPIOs via this connecthin:
//OLED_SDA -- GPIO4
//OLED_SCL -- GPIO15
//OLED_RST -- GPIO16

SSD1306  display(0x3c, 4, 15);


// WIFI_LoRa_32 ports

// GPIO5  -- SX1278's SCK
// GPIO19 -- SX1278's MISO
// GPIO27 -- SX1278's MOSI
// GPIO18 -- SX1278's CS
// GPIO14 -- SX1278's RESET
// GPIO26 -- SX1278's IRQ(Interrupt Request)

#define SS      18
#define RST     14
#define DI0     26
#define BAND    915E6 //915E6 433E6
bool DEBUG = false;



#define BUTTON_PIN_BITMASK 0x200000000 // 2^33 in hex
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  60*30        /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR int ProbeNumber = 3;
bool WakedByPIR;
String Packet;

/*
Method to print the reason by which ESP32
has been awaken from sleep
*/
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case 1  : Serial.println("Wakeup caused by external signal using RTC_IO"); WakedByPIR = 1; break;
    case 2  : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case 3  : Serial.println("Wakeup caused by timer"); WakedByPIR = 0; break;
    case 4  : Serial.println("Wakeup caused by touchpad"); break;
    case 5  : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.println("Wakeup was not caused by deep sleep"); WakedByPIR = 0; break;
  }
}

void setup(){

 
 if (DEBUG) {  
  pinMode(16,OUTPUT);
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50); 
  digitalWrite(16, HIGH); 

  
  
 
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(5,5,"LoRa Sender");
  display.display(); 
  
  }
  Serial.begin(115200);
  delay(50);
  SPI.begin(5,19,27,18);
  LoRa.setPins(SS,RST,DI0);
  Serial.println("LoRa Sender");
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initial OK!");
  //display.drawString(5,20,"LoRa Initializing OK!");
  //display.display();

  //Increment boot number and print it every reboot
  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));
  
  //display.clear();
  print_wakeup_reason();
  if (WakedByPIR == 1) Packet = "BR1"+String(ProbeNumber); else Packet = "BR0"+String(ProbeNumber);
  if (DEBUG)
  {
  display.setFont(ArialMT_Plain_16);
  display.drawString(3, 5, "Sending packet ");
  
  
  display.drawString(50, 30, Packet);
  display.display();
  }

  // send packet
  int i = 0;
  while (i<5) {  // Spread the word, five times. Server side is skipping some packets probably due to menu animations, will investigate
    
    
  LoRa.beginPacket();
  LoRa.print(Packet+String(i));
  LoRa.endPacket();
  delay(1000);
  i++; 
  }
  
  
  delay(1000);
  
  
  

  /*
  First we configure the wake up source
  We set our ESP32 to wake up for an external trigger.
  There are two types for ESP32, ext0 and ext1 .
  ext0 uses RTC_IO to wakeup thus requires RTC peripherals
  to be on while ext1 uses RTC Controller so doesnt need
  peripherals to be powered on.
  Note that using internal pullups/pulldowns also requires
  RTC peripherals to be turned on.
  */
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  gpio_pullup_en(GPIO_NUM_12);
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_12,1); //1 = High, 0 = Low

  //If you were to use ext1, you would use it like
  //esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK,ESP_EXT1_WAKEUP_ANY_HIGH);

  //Go to sleep now
  Serial.println("Going to sleep now");
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

void loop(){
  //This is not going to be called
}
