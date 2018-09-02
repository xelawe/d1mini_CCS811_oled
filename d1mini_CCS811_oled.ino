#include <cy_serdebug.h>
#include <cy_serial.h>

#include "ccs811_srvc.h"

#include "cy_wifi.h"
#include "cy_ota.h"
#include "cy_weather.h"

#include <Ticker.h>

#include <SFE_MicroOLED.h>  // Include the SFE_MicroOLED library

#define PIN_RESET 255  //
#define DC_JUMPER 0  // I2C Addres: 0 - 0x3C, 1 - 0x3D


MicroOLED oled(PIN_RESET, DC_JUMPER); // Example I2C declaration



const char* gc_hostname = "D1CCS811OLED";

Ticker TickMeas;
boolean gv_TickMeas;

void HandleTickMeas( ) {
  gv_TickMeas = true;
}

void setup() {
  cy_serial::start(__FILE__);

  // These three lines of code are all you need to initialize the
  // OLED and print the splash screen.

  oled.begin();     // Initialize the OLED
  oled.flipVertical(true);
  oled.flipHorizontal(true);
  oled.clear(PAGE); // Clear the display's internal memory
  oled.clear(ALL);  // Clear the library's display buffer
  oled.setFontType(0); // set font type 0, please see declaration in SFE_MicroOLED.cpp
  oled.setCursor(0, 0); // points cursor to x=0 y=0
  oled.println("WiFi init");
  oled.display();   // Display what's in the buffer (splashscreen)

  wifi_init(gc_hostname);

  oled.println("WiFi OK");
  oled.display();   // Display what's in the buffer (splashscreen)

  delay(500);

  oled.println("OTA init");
  oled.display(); 
  init_ota(gv_clientname);

  oled.println("CCS811 ini");
  oled.display();   // Display what's in the buffer (splashscreen)
  
  if ( init_ccs811() ){
    oled.println("CCS811 OK");
    oled.display();   // Display what's in the buffer (splashscreen)
  }
  
  do_sensor();
  gv_TickMeas = false;
  TickMeas.attach(10, HandleTickMeas);
}

void loop() {
  check_ota();

  if (gv_TickMeas == true) {

    do_sensor();

    do_display();

    gv_TickMeas = false;

  }

  delay(500);


}

void do_sensor() {

  get_ccs811();

}


void do_display() {
  oled.clear(PAGE);  // Clear the buffer
  oled.setFontType(0); // set font type 0, please see declaration in SFE_MicroOLED.cpp
  oled.setCursor(0, 0); // points cursor to x=0 y=0

  oled.print("CO2 ");
  oled.print(gv_co2);
  oled.println("ppm");
  //oled.println(" ");

  oled.print("TVOC ");
  oled.print(gv_tvoc);
  oled.println("ppb");
  //oled.println(" ");

  //        oled.println("Temp ");
  //        oled.print(temp);
  //        oled.println(" \tC");
  //        //oled.println(" ");

  oled.display(); // Draw the memory buffer
}







//void init_ccs811(){
//  Serial.println("CCS811 init");
//
//  oled.clear(PAGE); // Clear the display's internal memory
//  oled.clear(ALL);  // Clear the library's display buffer
//  oled.setFontType(0); // set font type 0, please see declaration in SFE_MicroOLED.cpp
//  oled.setCursor(0, 0); // points cursor to x=0 y=0
//  oled.println("CCS811 init");
//  oled.display();   // Display what's in the buffer (splashscreen)
//
//
//  //  if (!ccs.begin()) {
//  //    Serial.println("Failed to start sensor! Please check your wiring.");
//  //    while (1) {
//  //      delay(1000);
//  //    }
//  //}
//  //
//  //  //calibrate temperature sensor
//  //  while (!ccs.available());
//  //  float temp = ccs.calculateTemperature();
//  //  ccs.setTempOffset(temp - 25.0);
//
//
//  if(!sensor.begin(uint8_t(ADDR), uint8_t(WAKE_PIN))){
//    Serial.println("Initialization failed.");
//    while (1) {
//      delay(1000);
//    }
//  }
//
//  oled.clear(PAGE); // Clear the display's internal memory
//  oled.clear(ALL);  // Clear the library's display buffer
//  oled.setFontType(0); // set font type 0, please see declaration in SFE_MicroOLED.cpp
//  oled.setCursor(0, 0); // points cursor to x=0 y=0
//  oled.println("CCS811 OK");
//  oled.display();   // Display what's in the buffer (splashscreen)
//
//}

