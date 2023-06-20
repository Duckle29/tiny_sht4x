#pragma once

#if defined(ESP32) || defined(ESP8266)
#define _delay_ms(ms) delayMicroseconds((ms)*1000)
#elif defined(AVR)
#include <util/delay.h>
#endif

#include <inttypes.h>
#include <Wire.h>


typedef enum class SHT4x_cmd : uint8_t 
{
    meas_precision_high = 0xFD,
    meas_precision_medium = 0xF6,
    meas_precision_low = 0xE0,
    read_sn = 0x89,
    reset = 0x94,
    heat_200mw_1s = 0x39,
    heat_200mw_100ms = 0x32,
    heat_110mw_1s = 0x2F,
    heat_110mw_100ms = 0x24,
    heat_20mw_1s = 0x1E,
    heat_20mw_100ms = 0x15,
} SHT4x_cmd_t;


class SHT4x
{
    public:
    SHT4x();

    void init(uint8_t address= 0x44, TwoWire & = Wire);
    
    void send_cmd(SHT4x_cmd_t cmd);
    void send_cmd(uint8_t cmd);
    void poll_temp_humi(SHT4x_cmd_t cmd = SHT4x_cmd::meas_precision_high);
    int32_t get_temp();
    int32_t get_humi();
    uint32_t get_sn();

    // https://hackaday.com/2020/06/12/binary-math-tricks-shifting-to-divide-by-ten-aint-easy/
    uint32_t divu10(uint32_t n, uint8_t times=1);
    void get_response(uint8_t buff[], uint8_t timeout=10);
    
    private:
    uint8_t m_SHT4x_addr = 0x44;
    TwoWire * m_wire;

    uint16_t m_temp_ticks, m_humi_ticks;
    uint8_t calc_crc(uint16_t data);
    
};