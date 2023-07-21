#include "base64.h"

static const uint8_t base64enc_tab[64] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void RGBAtoRGBbase64(int64_t num_pixels, const uint32_t *buf, char *buf64) {
  // We receive RGBA pixel ( 32 bits )
  uint32_t pixel;
  uint8_t u1, u2, u3, u4;
  for (int64_t i = 0; i < num_pixels; i++) {
    pixel = buf[i]; // 32 bits, 4 bytes
    // c bit manipulation
    // 8 * 4 : xxxxxx xx.xxxx xxxx.xx xxxxxx xxxxxxxx ->
    // 6 * 4 : yyyyyy yyyyyy  yyyyyy  yyyyyy --------

    //      rgb -> encoded -> binary
    // 0x000000 -> AAAA    -> 000000 000000 000000 000000
    // 0xFF0000 -> /wAA    -> 111111 110000 000000 000000
    // 0x00FF00 -> AP8A    -> 000000 001111 111100 000000
    // 0x0000FF -> AAD/    -> 000000 000000 000011 111111

    // showing red
    u1 = ((pixel & 0x00FC0000) >> 18) & 0x3F;
    u2 = ((pixel & 0x0003F000) >> 12) & 0x3F;
    u3 = ((pixel & 0x00000FC0) >> 6) & 0x3F;
    u4 = ((pixel & 0x0000003F) >> 0) & 0x3F;

    // Write it to out buffer
    buf64[i * 4] = base64enc_tab[u1];
    buf64[i * 4 + 1] = base64enc_tab[u2];
    buf64[i * 4 + 2] = base64enc_tab[u3];
    buf64[i * 4 + 3] = base64enc_tab[u4];
  }
  // null terminator
  buf64[num_pixels * 4] = 0;
}
