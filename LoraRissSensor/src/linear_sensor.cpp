#include <main.h>

Adafruit_ADS1115 ads;  // Create an ADS1115 instance

bool initLinearSensor(){
ads.begin();
ads.setGain(GAIN_ONE); // 1x gain: +/- 4.096V range
return true;
}

uint16_t readLinearSensor()
{
  // Read ADC value from channel 0
  int16_t adc0 = ads.readADC_SingleEnded(0);
  
  // Convert ADC value to voltage
  float voltage = adc0 * (4.096 / 32767.0); 
  
  // Convert voltage to distance
  float distance = (voltage / REF_VOLTAGE) * MAX_DISTANCE;
  
  // Invert the distance
  distance = MAX_DISTANCE - distance;
  
  // Print the distance
  debug_print("Distance: ");
  debug_print(distance); // Print with two decimal places for precision
  debug_println(" mm");
  
  // Add a delay to avoid flooding the serial output
  delay(1000);
  return distance;
}