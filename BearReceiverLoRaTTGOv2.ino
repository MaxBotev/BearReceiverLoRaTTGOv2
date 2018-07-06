/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 by ThingPulse, Daniel Eichhorn
 * Copyright (c) 2018 by Fabrice Weinberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *
 */

#include "SoundData.h";
#include <XT_DAC_Audio.h>;

#include "SSD1306.h" //
 
#include "OLEDDisplayUi.h"

// Include custom images

#include "images.h"
#include <LoRa.h>


// SSD1306  display(0x3c, 4, 15); // for TTGO v1
SSD1306  display(0x3c, 21, 22);    // TTGO v2
OLEDDisplayUi ui     ( &display );
XT_DAC_Audio_Class DacAudio(25,0); 


XT_MultiPlay_Class AlarmSeq1;
XT_MultiPlay_Class AlarmSeq2;
XT_MultiPlay_Class AlarmSeq3;
XT_MultiPlay_Class AlarmSeq4;
XT_MultiPlay_Class AlarmSeq5;

XT_MultiPlay_Class ConnectSeq1;
XT_MultiPlay_Class ConnectSeq2;
XT_MultiPlay_Class ConnectSeq3;
XT_MultiPlay_Class ConnectSeq4;
XT_MultiPlay_Class ConnectSeq5;

XT_MultiPlay_Class LostSeq1;
XT_MultiPlay_Class LostSeq2;
XT_MultiPlay_Class LostSeq3;
XT_MultiPlay_Class LostSeq4;
XT_MultiPlay_Class LostSeq5;

//XT_Wav_Class StartUp(soundstartup);

XT_Wav_Class Sensor(soundsensor);
XT_Wav_Class Motion(soundmotion);
XT_Wav_Class Lost(soundlost);
XT_Wav_Class One(sound1);
XT_Wav_Class Two(sound2);
XT_Wav_Class Three(sound3);
XT_Wav_Class Four(sound4);
XT_Wav_Class Five(sound5);
XT_Wav_Class SensorConnected(soundconnected);





int DefaultProbeSleep = 30;



String Probe1 = "offline";
String Probe2 = "offline";
String Probe3 = "offline";
String Probe4 = "offline";
String Probe5 = "offline";
int P1Counts = 0;
int P2Counts = 0;
int P3Counts = 0;
int P4Counts = 0;
int P5Counts = 0;
int8_t P1Signal = 0;
int8_t P2Signal = 0;
int8_t P3Signal = 0;
int8_t P4Signal = 0;
int8_t P5Signal = 0;
unsigned long P1Alive = 0;
unsigned long P2Alive = 0;
unsigned long P3Alive = 0;
unsigned long P4Alive = 0;
unsigned long P5Alive = 0;
unsigned long P1Alarm = 0;
unsigned long P2Alarm = 0;
unsigned long P3Alarm = 0;
unsigned long P4Alarm = 0;
unsigned long P5Alarm = 0;
unsigned long LastTouched = 0;
unsigned long LastPacketReceived;
unsigned long NewPacketReceived;
String LastProbeReported = "";

bool ScreenIsOn = true;
int ScreenSleep = 1; // turn the screen off after 1 minute by default;
bool P1PIR;
bool P2PIR;
bool P3PIR;
bool P4PIR;
bool P5PIR;
int SnoozeTime = 0;
unsigned long SnoozeStart = 0;
bool Snooze = false;
bool RunOnce = false;
/*
This is un example howto use Touch Intrrerupts
The bigger the threshold, the more sensible is the touch
*/

int threshold = 60;
bool touch12detected = false;
bool touch13detected = false;
bool insideProbeDetails = false;

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
#define BAND    915E6 // 433E6


void gotTouch5(){
 touch12detected = true;
 
 LastTouched = millis();
 
 
}

void gotTouch4(){
 touch13detected = true;
 // LastTouched = millis();

}

void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
  //display->setTextAlignment(TEXT_ALIGN_RIGHT);
  //display->setFont(ArialMT_Plain_10);
  //display->drawString(128, 0, String(millis()));
}

void drawFrame1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_24);
  display->drawString(64 + x, 0 + y, "BearWatch");
  display->setFont(ArialMT_Plain_16);
  display->drawString(64 + x, 30 + y, "activated");
}

void drawFrame2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // Demonstrates the 3 included default sizes. The fonts come from SSD1306Fonts.h file
  // Besides the default fonts there will be a program to convert TrueType fonts into this format
  //display->setTextAlignment(TEXT_ALIGN_LEFT);
  //display->setFont(ArialMT_Plain_10);
  //display->drawString(0 + x, 10 + y, "Arial 10");
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(0 + x, 0 + y, "P1: "+Probe1); if (Probe1 != "offline") display->drawString(80 + x, 0 + y, "Counts: "+String(P1Counts));
  display->drawString(0 + x, 11 + y, "P2: "+Probe2);if (Probe2 != "offline") display->drawString(80 + x, 11 + y, "Counts: "+String(P2Counts));
  display->drawString(0 + x, 22 + y, "P3: "+Probe3);if (Probe3 != "offline") display->drawString(80 + x, 22 + y, "Counts: "+String(P3Counts));
  display->drawString(0 + x, 33 + y, "P4: "+Probe4);if (Probe4 != "offline") display->drawString(80 + x, 33 + y, "Counts: "+String(P4Counts));
  display->drawString(0 + x, 44 + y, "P5: "+Probe5);if (Probe5 != "offline") display->drawString(80 + x, 44 + y, "Counts: "+String(P5Counts));

  //display->setFont(ArialMT_Plain_24);
  //display->drawString(0 + x, 34 + y, "Arial 24");
}



void drawSnooze(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->setFont(ArialMT_Plain_24);
  display->drawString(64 + x, 0 + y, "Snooze");
  display->setFont(ArialMT_Plain_16);
  display->drawString(64 + x, 30 + y, String(SnoozeTime)+" min");
}

void drawDetailFrame1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_16);
  if (P1PIR) display->drawString(0 + x, 0 + y, "P1: motion!"); else display->drawString(0 + x, 0 + y, "P1: "+Probe1);
  if (Probe1 != "offline") {
     // display signal strenght and last event if it's not offline
     uint8_t minutes = (millis()-P1Alarm) / 1000 / 60;
     display->setFont(ArialMT_Plain_10);
     display->drawString(0 + x, 20 + y, "Signal quality: "+String(P1Signal)+"%"); 
     display->drawString(0 + x, 32 + y, "Last event: "+String(minutes)+" min ago"); 
     display->drawString(0 + x, 44 + y, "Counts: "+String(P1Counts)); 
  
   }
  }
void drawDetailFrame2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_16);
  if (P2PIR) display->drawString(0 + x, 0 + y, "P2: motion!"); else display->drawString(0 + x, 0 + y, "P2: "+Probe2);
  if (Probe2 != "offline") {
     // display signal strenght and last event if it's not offline
     uint8_t minutes = (millis()-P2Alarm) / 1000 / 60;
     display->setFont(ArialMT_Plain_10);
     display->drawString(0 + x, 20 + y, "Signal quality: "+String(P2Signal)+"%"); 
     display->drawString(0 + x, 32 + y, "Last event: "+String(minutes)+" min ago"); 
     display->drawString(0 + x, 44 + y, "Counts: "+String(P2Counts)); 
  
   }
  }

void drawDetailFrame3(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  
 display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_16);
  if (P3PIR) display->drawString(0 + x, 0 + y, "P3: motion!"); else display->drawString(0 + x, 0 + y, "P3: "+Probe3);
  if (Probe3 != "offline") {
     // display signal strenght and last event if it's not offline
     uint8_t minutes = (millis()-P3Alarm) / 1000 / 60;
     display->setFont(ArialMT_Plain_10);
     display->drawString(0 + x, 20 + y, "Signal quality: "+String(P3Signal)+"%"); 
     display->drawString(0 + x, 32 + y, "Last event: "+String(minutes)+" min ago"); 
     display->drawString(0 + x, 44 + y, "Counts: "+String(P3Counts)); 
  
  
   }
  }

void drawDetailFrame4(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_16);
  if (P4PIR) display->drawString(0 + x, 0 + y, "P4: motion!"); else display->drawString(0 + x, 0 + y, "P4: "+Probe4);
  if (Probe4 != "offline") {
     // display signal strenght and last event if it's not offline
     uint8_t minutes = (millis()-P4Alarm) / 1000 / 60;
     display->setFont(ArialMT_Plain_10);
     display->drawString(0 + x, 20 + y, "Signal quality: "+String(P4Signal)+"%"); 
     display->drawString(0 + x, 32 + y, "Last event: "+String(minutes)+" min ago"); 
     display->drawString(0 + x, 44 + y, "Counts: "+String(P4Counts)); 
  
  
   }
  }

void drawDetailFrame5(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_16);
  if (P5PIR) display->drawString(0 + x, 0 + y, "P5: motion!"); else display->drawString(0 + x, 0 + y, "P5: "+Probe5);
  if (Probe5 != "offline") {
     // display signal strenght and last event if it's not offline
     uint8_t minutes = (millis()-P5Alarm) / 1000 / 60;
     display->setFont(ArialMT_Plain_10);
     display->drawString(0 + x, 20 + y, "Signal quality: "+String(P5Signal)+"%"); 
     display->drawString(0 + x, 32 + y, "Last event: "+String(minutes)+" min ago"); 
     display->drawString(0 + x, 44 + y, "Counts: "+String(P5Counts)); 
  
  
   }
  }
  //display->setFont(ArialMT_Plain_24);
  //display->drawString(0 + x, 34 + y, "Arial 24");


// This array keeps function pointers to all frames
// frames are the single views that slide in
FrameCallback frames[] = { drawFrame1, drawFrame2,  drawDetailFrame1, drawDetailFrame2, drawDetailFrame3, drawDetailFrame4, drawDetailFrame5, drawSnooze };

// how many frames are there?
int frameCount = 8;
int currentFrame = 1;

// Overlays are statically drawn on top of a frame eg. a clock
//OverlayCallback overlays[] = { msOverlay };
//int overlaysCount = 1;

void horizontalMenu() {
  //FrameCallback frames[] = { drawFrame1, drawFrame2, drawFrame3, drawFrame4, drawFrame5 };
  // ui.setIndicatorPosition(BOTTOM);

  // Defines where the first frame is located in the bar.
  ui.setIndicatorDirection(LEFT_RIGHT);

  // You can change the transition that is used
  // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
  ui.setFrameAnimation(SLIDE_LEFT);

  // Add frames
  ui.setFrames(frames, frameCount);

  // Add overlays
  //ui.setOverlays(overlays, overlaysCount);

  // Initialising the UI will init the display too.
  ui.init();

  display.flipScreenVertically();
  ui.disableAutoTransition();
}

void setup() {

  
  
  ConnectSeq1.AddPlayItem(&SensorConnected); ConnectSeq1.AddPlayItem(&Sensor);ConnectSeq1.AddPlayItem(&One); 
  ConnectSeq2.AddPlayItem(&SensorConnected); ConnectSeq2.AddPlayItem(&Sensor);ConnectSeq2.AddPlayItem(&Two); 
  ConnectSeq3.AddPlayItem(&SensorConnected); ConnectSeq3.AddPlayItem(&Sensor);ConnectSeq3.AddPlayItem(&Three); 
  ConnectSeq4.AddPlayItem(&SensorConnected); ConnectSeq4.AddPlayItem(&Sensor);ConnectSeq4.AddPlayItem(&Four); 
  ConnectSeq5.AddPlayItem(&SensorConnected); ConnectSeq5.AddPlayItem(&Sensor);ConnectSeq5.AddPlayItem(&Five); 
ConnectSeq1.Init();ConnectSeq2.Init();ConnectSeq3.Init();ConnectSeq4.Init();ConnectSeq5.Init();
  AlarmSeq1.AddPlayItem(&Motion); AlarmSeq1.AddPlayItem(&One); 
  AlarmSeq2.AddPlayItem(&Motion); AlarmSeq2.AddPlayItem(&Two); 
  AlarmSeq3.AddPlayItem(&Motion); AlarmSeq3.AddPlayItem(&Three); 
  AlarmSeq4.AddPlayItem(&Motion); AlarmSeq4.AddPlayItem(&Four); 
  AlarmSeq5.AddPlayItem(&Motion); AlarmSeq5.AddPlayItem(&Five); 

  LostSeq1.AddPlayItem(&Lost); LostSeq1.AddPlayItem(&Sensor); LostSeq1.AddPlayItem(&One); 
  LostSeq2.AddPlayItem(&Lost); LostSeq2.AddPlayItem(&Sensor); LostSeq2.AddPlayItem(&Two); 
  LostSeq3.AddPlayItem(&Lost); LostSeq3.AddPlayItem(&Sensor); LostSeq3.AddPlayItem(&Three); 
  LostSeq4.AddPlayItem(&Lost); LostSeq4.AddPlayItem(&Sensor); LostSeq4.AddPlayItem(&Four); 
  LostSeq5.AddPlayItem(&Lost); LostSeq5.AddPlayItem(&Sensor); LostSeq5.AddPlayItem(&Five); 

  
  Serial.begin(115200);
  delay(500);
  
 // DacAudio.Play(&StartUp);
  while (!Serial);
  SPI.begin(5,19,27,18);
  LoRa.setPins(SS,RST,DI0);
  
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initial OK!");
 // pinMode(16,OUTPUT);
 // digitalWrite(16, LOW);    // set GPIO16 low to reset OLED for TTGO v1
 // delay(50); 
 // digitalWrite(16, HIGH);
  touchAttachInterrupt(T5, gotTouch5, threshold);
  touchAttachInterrupt(T4, gotTouch4, threshold);
	// The ESP is capable of rendering 60fps in 80Mhz mode
	// but that won't give you much time for anything else
	// run it in 160Mhz mode or just set it to 30 fps
  ui.setTargetFPS(30);

	// Customize the active and inactive symbol
  ui.setActiveSymbol(activeSymbol);
  ui.setInactiveSymbol(inactiveSymbol);

  horizontalMenu();

  

}




void HandlePacket(String packet, int8_t Signal){
 bool SameProbe;
 bool LongEnough;
 bool Alarmed;
 if (LastProbeReported == packet.substring(3,4)) SameProbe = true; else SameProbe=false;
 if ((NewPacketReceived - LastPacketReceived) > 20*1000) LongEnough = true; else LongEnough = false;
 if (packet.substring(2,3) == "1") Alarmed = true; else Alarmed = false;
 
 Serial.println("Alarmed: "+String(Alarmed));
 
  if (packet.substring(0,2) = "BR") { // the packet was for us
     
     if ((SameProbe && LongEnough) or !SameProbe)
     { 
     
      Serial.print(packet.substring(2,4));
      LastProbeReported = packet.substring(3,4);
      Serial.println(" Probe: "+LastProbeReported);

      if (LastProbeReported == "1") { P1Signal = Signal;if (Probe1 !="online" && !Snooze) ProbeConnected(1); Probe1 = "online";  P1Alive=millis(); if (!Alarmed) P1PIR=0; else {if (!Snooze) Alarm(1);BearFlash(1); LastTouched = millis();P1Alarm = millis();P1PIR = 1 ; P1Counts ++; };} 
      if (LastProbeReported == "2") { P2Signal = Signal;if (Probe2 !="online" && !Snooze) ProbeConnected(2); Probe2 = "online";  P2Alive=millis(); if (!Alarmed) P2PIR=0; else {if (!Snooze) Alarm(2);BearFlash(2); LastTouched = millis();P2Alarm = millis();P2PIR = 1 ; P2Counts ++; };} 
      if (LastProbeReported == "3") { P3Signal = Signal;if (Probe3 !="online" && !Snooze) ProbeConnected(3); Probe3 = "online";  P3Alive=millis(); if (!Alarmed) P3PIR=0; else {if (!Snooze) Alarm(3);BearFlash(3); LastTouched = millis();P3Alarm = millis();P3PIR = 1 ; P3Counts ++; };} 
      if (LastProbeReported == "4") { P4Signal = Signal;if (Probe4 !="online" && !Snooze) ProbeConnected(4); Probe4 = "online";  P4Alive=millis(); if (!Alarmed) P4PIR=0; else {if (!Snooze) Alarm(4);BearFlash(4); LastTouched = millis();P4Alarm = millis();P4PIR = 1 ; P4Counts ++; };} 
      if (LastProbeReported == "5") { P5Signal = Signal;if (Probe5 !="online" && !Snooze) ProbeConnected(5); Probe5 = "online";  P5Alive=millis(); if (!Alarmed) P5PIR=0; else {if (!Snooze) Alarm(5);BearFlash(5); LastTouched = millis();P5Alarm = millis();P5PIR = 1 ; P5Counts ++; };} 
  
  // Update screen 
     }
}
}


void ProbeDisconnected(int ProbeNum) {

   switch (ProbeNum)
  {
  
   case 1: DacAudio.Play(&LostSeq1); break;
   case 2: DacAudio.Play(&LostSeq2); break;
   case 3: DacAudio.Play(&LostSeq3); break;
   case 4: DacAudio.Play(&LostSeq4); break;
   case 5: DacAudio.Play(&LostSeq5); break;
  }
}

void ProbeConnected(int ProbeNum) {

 
  
   switch (ProbeNum)
  {
  
   case 1: DacAudio.Play(&ConnectSeq1); break;
   case 2: DacAudio.Play(&ConnectSeq2); break;
   case 3: DacAudio.Play(&ConnectSeq3); break;
   case 4: DacAudio.Play(&ConnectSeq4); break;
   case 5: DacAudio.Play(&ConnectSeq5); break;
  }
  
  
}

void Alarm(int Zone) {


   switch (Zone)
   
  {
   case 1: DacAudio.Play(&AlarmSeq1); break;
   case 2: DacAudio.Play(&AlarmSeq2); break;
   case 3: DacAudio.Play(&AlarmSeq3); break;
   case 4: DacAudio.Play(&AlarmSeq4); break;
   case 5: DacAudio.Play(&AlarmSeq5); break;
  }
 
  
}




void BearFlash(int WhichBear) {
  int i = 0;
  ScreenIsOn = true; 
  while (i<5) {
    i++;
  
  display.init();
  display.resetDisplay();
  display.flipScreenVertically();
  
  Serial.println("Bear #"+String(WhichBear));
  switch (WhichBear)
  {
  
   case 1: display.drawXbm(0, 0, Bear_width, Bear_height, Bear1_Logo_bits); break;
   case 2: display.drawXbm(0, 0, Bear_width, Bear_height, Bear2_Logo_bits); break;
   case 3: display.drawXbm(0, 0, Bear_width, Bear_height, Bear3_Logo_bits); break;
   case 4: display.drawXbm(0, 0, Bear_width, Bear_height, Bear4_Logo_bits); break;
   case 5: display.drawXbm(0, 0, Bear_width, Bear_height, Bear5_Logo_bits); break;
  }
  
  display.display();
  
  delay(500);
  display.displayOff();
  
  delay(500);
  display.displayOn();
  
  display.displayOn();
  //
 
 ui.init();

 display.flipScreenVertically();
 ui.transitionToFrame(1+WhichBear); 
}
}

int8_t getSignalQuality() {
  int32_t dbm = LoRa.packetRssi();
  if(dbm <= -100) {
      return 0;
  } else if(dbm >= -50) {
      return 100;
  } else {
      return 2 * (dbm + 100);
  }
}


void loop() {
  DacAudio.FillBuffer();  
 
  
 int packetSize = LoRa.parsePacket();
    if (packetSize) {
    // received a packets
    Serial.print("Received packet. ");
    // read packet
    while (LoRa.available()) {
    String data = LoRa.readString();
    LastPacketReceived = NewPacketReceived;
    NewPacketReceived = millis();
    
    HandlePacket(data,getSignalQuality());
    Serial.println(data);
    }
    }
if ( Probe1 == "online" && P1PIR && (millis()-P1Alive) > 10000 ) P1PIR = 0;  
if ( Probe2 == "online" && P2PIR && (millis()-P2Alive) > 10000 ) P2PIR = 0;  
if ( Probe3 == "online" && P3PIR && (millis()-P3Alive) > 10000 ) P3PIR = 0;  
if ( Probe4 == "online" && P4PIR && (millis()-P4Alive) > 10000 ) P4PIR = 0;  
if ( Probe5 == "online" && P5PIR && (millis()-P5Alive) > 10000 ) P5PIR = 0;   
if ( ((millis()-P1Alive) > DefaultProbeSleep * 60000 + 20000 ) && (Probe1=="online")) {Probe1 = "lost signal"; if (!Snooze) ProbeDisconnected(1);};
if ( ((millis()-P2Alive) > DefaultProbeSleep * 60000 + 20000 ) && (Probe2=="online")) {Probe2 = "lost signal"; if (!Snooze) ProbeDisconnected(2);};
if ( ((millis()-P3Alive) > DefaultProbeSleep * 60000 + 20000 ) && (Probe3=="online")) {Probe3 = "lost signal"; if (!Snooze) ProbeDisconnected(3);};
if ( ((millis()-P4Alive) > DefaultProbeSleep * 60000 + 20000 ) && (Probe4=="online")) {Probe4 = "lost signal"; if (!Snooze) ProbeDisconnected(4);};
if ( ((millis()-P5Alive) > DefaultProbeSleep * 60000 + 20000 ) && (Probe5=="online")) {Probe5 = "lost signal"; if (!Snooze) ProbeDisconnected(5);};

if ( millis()-LastPacketReceived  > (5 * 1000)) LastProbeReported = ""; // will igmnore same probe for 5 seconds 
  
  int remainingTimeBudget = ui.update();

  if (remainingTimeBudget > 0) {
    
    // You can do some work here
    // Don't do stuff if you are below your
    // time budget.
  
    if ((millis() - LastTouched) > ScreenSleep * 1000 * 60) {display.displayOff(); ScreenIsOn = false; };
    
    if ((millis()-SnoozeStart) > 1 * 1000 *60) { SnoozeTime --; SnoozeStart = millis();}
    if (SnoozeTime <= 0) {SnoozeTime = 0; Snooze = false;};
    
    if(touch12detected){
     // next button
    touch12detected = false;
    
    if (!ScreenIsOn) {display.displayOn();ui.init();display.flipScreenVertically();ScreenIsOn = true;} else ui.nextFrame();
    Serial.println("Next button press detected ");
  }
  if(touch13detected){
    // Snooze button
    touch13detected = false;
    
    if (!ScreenIsOn) {display.displayOn();ui.init();display.flipScreenVertically();ScreenIsOn = true;} else ui.transitionToFrame(7);
    //ui.update();
        // Change snooze time
    if ((millis()-LastTouched) > 500 ) {   
      if (!Snooze) Snooze = true;
      if (SnoozeTime < 30) {SnoozeTime = SnoozeTime +5;} else SnoozeTime = SnoozeTime + 30;
      if (SnoozeTime > 5*60) { SnoozeTime = 0; Snooze = false;};
      LastTouched = millis();
      SnoozeStart = millis(); 
      Serial.println("Snooze button press detected");
    }
  }
    delay(remainingTimeBudget);
  } 
}
