# tiny_sht4x
A library to interface with Sensirion SHT4x sensors without floats or divisions

I put this library together as I needed to use an SHT40 sensor on an ATTiny414 which has a very limited 256B of SRAM as well as 4KB of flash.
The basic example without `Serial` compiles to an additional 93 bytes of SRAM and 1282 bytes of code. While this isn't great, it fits. If anyone wants to further optimize it, be my guest :)
