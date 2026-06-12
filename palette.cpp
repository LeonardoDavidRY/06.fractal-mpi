#include "palette.h"

// forma de invertir los bytes
// los >> o << son desplazamientos de bits derecha o izquierda
uint32_t _bswap32(uint32_t a)
{
    return ((a & 0x000000FF) << 24) |
           ((a & 0x0000FF00) << 8) |
           ((a & 0x00FF0000) >> 8) |
           ((a & 0xFF000000) >> 24);
}
std::vector<uint32_t> color_ramp = {
    _bswap32(0xFF1010FF),
    _bswap32(0xED1B19FF),
    _bswap32(0xE01123FF),
    _bswap32(0xD11123FF),
    _bswap32(0xC11237FF),
    _bswap32(0xB21341FF),
    _bswap32(0xA3134BFF),
    _bswap32(0x931455FF),
    _bswap32(0xB4145EFF),
    _bswap32(0x751568FF),
    _bswap32(0x651672FF),
    _bswap32(0X561672FF),
    _bswap32(0x471786FF),
    _bswap32(0x371790FF),
    _bswap32(0x28189AFF),
    _bswap32(0x1919A4FF)
};