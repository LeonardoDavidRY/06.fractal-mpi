#include "palette.h"
/*
bswap32(0x30123BFF),
bswap32(0x40419DFF),
bswap32(0x4670E5FF),
bswap32(0x3F9FF9FF),
bswap32(0x2AC8DAFF),
bswap32(0x25E4A7FF),
bswap32(0x4CF66EFF),
bswap32(0x87FC47FF),
bswap32(0xB9F338FF),
bswap32(0xDEDD37FF),
bswap32(0xF5BA36FF),
bswap32(0xFB8B28FF),
bswap32(0xEB5717FF),
bswap32(0xCC2D04FF),
bswap32(0xA10F01FF),
bswap32(0x7A0403FF),
*/

uint32_t bswap32(uint32_t a)
{
    return ((a & 0x000000FF) << 24) |
           ((a & 0x0000FF00) << 8) |
           ((a & 0x00FF0000) >> 8) |
           ((a & 0xFF000000) >> 24);
}

std::vector<uint32_t> color_ramp = {
    bswap32(0xFF1010FF),
    bswap32(0xF31017FF),
    bswap32(0xE8101EFF),
    bswap32(0xDC1126FF),
    bswap32(0xD1112DFF),
    bswap32(0xC51235FF),
    bswap32(0xBA123CFF),
    bswap32(0xAE1343FF),
    bswap32(0xA3134BFF),
    bswap32(0x971452FF),
    bswap32(0x8C145AFF),
    bswap32(0x801461FF),
    bswap32(0x751568FF),
    bswap32(0x691570FF),
    bswap32(0x5E1677FF),
    bswap32(0x52167FFF),
    bswap32(0x471786FF),
    bswap32(0x3B178DFF),
    bswap32(0x301895FF),
    bswap32(0x24189CFF),
    bswap32(0x1919A4FF)};

std::vector<uint32_t> color_ramp2 = {
    bswap32(0x30123BFF),
    bswap32(0x40419DFF),
    bswap32(0x4670E5FF),
    bswap32(0x3F9FF9FF),
    bswap32(0x2AC8DAFF),
    bswap32(0x25E4A7FF),
    bswap32(0x4CF66EFF),
    bswap32(0x87FC47FF),
    bswap32(0xB9F338FF),
    bswap32(0xDEDD37FF),
    bswap32(0xF5BA36FF),
    bswap32(0xFB8B28FF),
    bswap32(0xEB5717FF),
    bswap32(0xCC2D04FF),
    bswap32(0xA10F01FF),
    bswap32(0x7A0403FF)};