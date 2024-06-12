#include <main.h>

Adafruit_ADS1115 ads;  // Create an ADS1115 instance

bool initLinearSensor(){
ads.begin();
ads.setGain(GAIN_ONE); // 1x gain: +/- 4.096V range
return true;
}

int16_t readLinearSensor()
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
  
  return adc0;
}

uint8_t readLinearSensor_8()
{
  // Read ADC value from channel 0
  int16_t adc0 = ads.readADC_SingleEnded(0);
  debug_print("ADC: ");
  debug_println(adc0);
  uint8_t scaled_value;
  
  // Scale the ADC value to fit into uint8_t
  //uint8_t scaled_value = (uint8_t)(adc0);
  //uint8_t scaled_value = (uint8_t)(((float)(adc0 - 0) / (255 - 0)) * 255.0);
/* 
      // Ensure the value is within the expected range
    if (adc0 <= -32767) {
        scaled_value = 0;  // if the value is less than or equal to -32767, map it to 0
    } else if (adc0 >= 32767) {
        scaled_value = 255;  // if the value is greater than or equal to 32767, map it to 255
    } else {
        // Perform linear mapping
        scaled_value = static_cast<uint8_t>((adc0 + 32767) / 256);
    } */

    if (adc0 >= 26321) {
        return 255;  // if the value is greater than or equal to 26321, map it to 255
    } else {
        // Perform linear mapping
        scaled_value = static_cast<uint8_t>((adc0 * 255) / 26321);
    }

  
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
  
  return scaled_value;
}