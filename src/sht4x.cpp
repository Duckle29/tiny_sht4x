#include "sht4x.h"

SHT4x::SHT4x()
{}


bool SHT4x::init(uint8_t address, TwoWire &wirePort)
{
    m_SHT4x_addr = address;
    m_wire = &wirePort;   
}


void SHT4x::get_response(uint8_t buff[], uint8_t timeout)
{
    // Wait for reading to be ready. Can take up to 8.3ms
    for (uint8_t tries=0; tries<timeout; ++tries)
    {
        if (! m_wire->requestFrom(m_SHT4x_addr, 6))
        {
            _delay_ms(1);
            continue;
        }
        m_wire->readBytes(buff, 6);
    }
}

void SHT4x::send_cmd(SHT4x_cmd_t cmd)
{
    send_cmd((uint8_t) cmd);
}

void SHT4x::send_cmd(uint8_t cmd)
{
    m_wire->beginTransmission(m_SHT4x_addr);
    m_wire->write((uint8_t) cmd);
    m_wire->endTransmission();
}

void SHT4x::poll_temp_humi(SHT4x_cmd_t cmd)
{
    
    send_cmd(cmd);

    uint8_t buff[6];
    get_response(buff);

    uint16_t tmp = (buff[0] << 8) | buff[1];
    if (calc_crc(tmp) == buff[2])
    {
        m_temp_ticks = tmp;
    }

    tmp = (buff[3] << 8) | buff[4];
    if (calc_crc(tmp) == buff[5])
    {
        m_humi_ticks = tmp;
    }
}

int32_t SHT4x::get_humi()
{
    /* Formula from the datasheet is:
            (125 * ticks) / ((2^16)-1) - 6

        I will simplify the division to 2^16, which is a 0.002% error
        I'll also represent this as a fixed point number with 2 decimal places
        so the final formula is:
            (12500 * ticks / 2^16) - 600
    */
   
    int32_t tmp = m_humi_ticks;
    tmp *= 12500;
    tmp = (tmp >> 16);
    tmp -= 600;

    return tmp;
}

int32_t SHT4x::get_temp()
{
    /* Formula from the datasheet is:
            (175 * ticks) / ((2^16)-1) - 45

        I will simplify the division to 2^16, which is a 0.002% error
        I'll also represent this as a fixed point number with 2 decimal places
        so the final formula is:
            (17500 * ticks / 2^16) - 4500
    */

    uint32_t tmp = m_temp_ticks;
    tmp *= 17500;
    tmp = (tmp >> 16);
    tmp -= 4500;

    return tmp;

}

uint32_t SHT4x::get_sn()
{
    send_cmd(SHT4x_cmd::read_sn);

    uint8_t buff[6];
    get_response(buff);

    uint32_t res;
    res = (buff[0] << 8) | buff[1];
    
    // Check CRC, store result in CRC byte
    buff[2] = calc_crc(res & 0xFFFF) == buff[2];

    res = (res << 16) | (buff[3] << 8) | buff[4];

    if (calc_crc(res & 0xFFFF) == buff[5] && buff[2])
    {
        return res;
    }
    else
    {
        return 0;
    }
}

// https://hackaday.com/2020/06/12/binary-math-tricks-shifting-to-divide-by-ten-aint-easy/
unsigned SHT4x::divu10(unsigned n) {
  unsigned q, r;
  q = (n >> 1) + (n >> 2);
  q = q + (q >> 4);
  q = q + (q >> 8);
  q = q + (q >> 16);
  q = q >> 3;
  r = n - (((q << 2) + q) << 1);
  return q + (r > 9);
}

uint8_t SHT4x::calc_crc(uint16_t data)
{
    if (data == NULL)
    {
        return 0xff;
    }
    
    uint8_t crc = 0xFF;
    
    for (int8_t i=1; i>=0; --i)
    {
        crc ^= (data >> i*8) & 0xFF;
        for (uint8_t k=0; k<8; k++)
        {
            crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
            
        }
    }    
    return crc;  
}