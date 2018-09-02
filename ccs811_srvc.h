/*
  ccs811demo.ino - Demo sketch printing results of the CCS811 digital gas sensor for monitoring indoor air quality from ams.
  Created by Maarten Pennings 2017 Dec 11
*/


#include <Wire.h>    // I2C library
#include "ccs811.h"  // CCS811 library


CCS811 ccs811(D3); // nWAKE to D3

uint16_t gv_co2;
uint16_t gv_tvoc;
uint16_t gv_errstat;

int gv_count;
#define gc_max_cnt 100


void print_versions() {
  uint8_t hw_version;
  uint16_t fw_boot_version;
  uint16_t fw_app_version;
  bool ok = ccs811.versions(&hw_version, &fw_boot_version, &fw_app_version);
  if ( ok ) {
    Serial.print("init: CCS811 versions: ");
    Serial.print("hw 0x"); Serial.print(hw_version, HEX);
    Serial.print(", fw_boot 0x"); Serial.print(fw_boot_version, HEX);
    Serial.print(", fw_app 0x"); Serial.print(fw_app_version, HEX);
    Serial.println("");
  } else {
    Serial.println("init: CCS811 versions FAILED");
  }
}

bool reset_ccs881() {

  // Enable CCS811
  bool ok = ccs811.begin();
  if ( !ok ) Serial.println("init: CCS811 begin FAILED");
  print_versions();

  // Start measuring
  ok = ccs811.start(CCS811_MODE_1SEC);
  if ( !ok) Serial.println("init: CCS811 start FAILED");


  gv_count = 0;
  return ok;
}

bool init_ccs811() {

  Serial.println("");
  Serial.println("Starting CCS811");

  // Enable I2C for ESP8266 NodeMCU boards [VDD to 3V3, GND to GND, nWAKE to D3, SDA to D2, SCL to D1]
  Wire.begin(/*SDA*/D2,/*SCL*/D1);

  gv_count = gc_max_cnt;

  return reset_ccs881();

}


void get_ccs811() {

  uint16_t eco2;
  uint16_t etvoc;
  uint16_t errstat;
  uint16_t raw;

  // Read
  ccs811.read(&eco2, &etvoc, &errstat, &raw); // Note, I2C errors are also in errstat

  // Check if errstat flags denote VALID&NEW or OLD|ERROR
  bool valid_and_new = ( (errstat & CCS811_ERRSTAT_OKS) == CCS811_ERRSTAT_OKS )  &&  ( (errstat & CCS811_ERRSTAT_ERRORS) == 0 );

  // Print
  Serial.print("CCS811: ");

  Serial.print("eco2=");
  Serial.print(eco2);
  Serial.print(" ppm,  ");

  Serial.print("etvoc=");
  Serial.print(etvoc);
  Serial.print(" ppb,  ");

  Serial.print("errstat=");
  Serial.print(errstat, HEX);
  Serial.print("=");
  Serial.print(ccs811.errstat_str(errstat));
  Serial.print( valid_and_new ? "=valid&new,  " : "=ERROR|OLD,  " );

  if (valid_and_new) {
    gv_co2 = eco2;
    gv_tvoc = etvoc;
    gv_errstat = errstat;
  }

  Serial.print("raw6=");
  Serial.print(raw / 1024);
  Serial.print(" uA, ");

  Serial.print("raw10=");
  Serial.print(raw % 1024);
  Serial.print(" ADC");

  Serial.println();

  gv_count++;
  if (gv_count > gc_max_cnt) {

    reset_ccs881();

  }
}
