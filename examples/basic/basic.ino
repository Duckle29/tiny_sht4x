#include <Wire.h>
#include <sht4x.h>

SHT4x sht40;

void setup()
{
    Serial.begin(9600);
    Wire.begin();
    sht40.init();
}

void loop()
{
    sht40.poll_temp_humi();
    
    // Temp and humidity are returned as fixed point notation with two decimal places 
    // For example: 3000 is 30.00°C or 30.00% relative humidity
    // Floats are expensive, and I only have 4KB of flash
    Serial.print("T: "); Serial.print(sht40.get_temp());
    Serial.print("\tRH: "); Serial.print(sht40.get_humi());
}