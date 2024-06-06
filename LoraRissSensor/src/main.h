#include <Arduino.h>

uint8_t getBatteryLevel();

#define DEBUG // Herauskommentieren, wenn keine serielle Ausgabe erwünscht wird

#ifdef DEBUG
#define debug_print(x) Serial.print(x)
#define debug_println(x) Serial.println(x)
#define debug_begin(x) Serial.begin(x)
#else
#define debug_print(x)
#define debug_println(x)
#define debug_begin(x)
#endif

#include <LoRaWan_APP.h> //LoraWan

bool initLorawanModule();
void prepareTxFrame(uint8_t port);
void loraLoopHandler();

#include <Wire.h>
#include <Adafruit_ADS1X15.h>



// Constants
const float REF_VOLTAGE = 3.3; // Reference voltage
const float MAX_DISTANCE = 50.0; // Maximum distance in mm

bool initLinearSensor();
uint16_t readLinearSensor();
//uint16_t distance;


