#include <Wire.h>
#include <sht4x.h>

SHT4x sht_special;

void setup()
{
    Serial.begin(9600);
    Wire.begin();
    sht_special.init(0x45, Wire); // Some special sht4x with a different address, maybe a different TwoWire
}

void loop()
{
    sht_special.poll_temp_humi();
    
    // Temp and humidity are returned as fixed point notation with two decimal places 
    // For example: 3000 is 30.00°C or 30.00% relative humidity
    // Floats are expensive, and I only have 4KB of flash
    Serial.print("T: "); Serial.print(sht_special.get_temp());
    Serial.print("\tRH: "); Serial.print(sht_special.get_humi());

    // If you don't care about fractional degrees and want to divide by 100,
    // There's the dope bit-twiddling div10 from this hackaday article
    // https://hackaday.com/2020/06/12/binary-math-tricks-shifting-to-divide-by-ten-aint-easy/
    uint8_t temp = sht_special.divu10(sht_special.divu10(sht_special.get_temp()));

    uint32_t sensor_sn = sht_special.get_sn();

    // Remember to never exceed 10% duty-cycle of the heaters
    sht_special.poll_temp_humi(SHT4x_cmd::heat_110mw_1s);

    // Software reset the sensor
    sht_special.send_cmd(SHT4x_cmd::reset);
}