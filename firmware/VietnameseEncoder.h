#pragma once
#include <Arduino.h>

/**
 * Decodes UTF-8 Vietnamese strings into single-byte character indices (0x80..0xFF)
 * matching VietnameseSansBold12pt8b and VietnameseSans9pt8b.
 */
inline String utf8viet(const char* source) {
  String target = "";
  int i = 0;
  int len = strlen(source);

  while (i < len) {
    uint8_t c1 = (uint8_t)source[i++];
    if (c1 < 0x80) {
      target += (char)c1;
    } else if ((c1 & 0xE0) == 0xC0 && i < len) { // 2-byte UTF-8
      uint8_t c2 = (uint8_t)source[i++];
      uint16_t code = (c1 << 8) | c2;
      uint8_t mapped = 0;
      switch (code) {
        case 0xC380: mapped = 0xC3; break; // À
        case 0xC381: mapped = 0xC4; break; // Á
        case 0xC382: mapped = 0xCD; break; // Â
        case 0xC383: mapped = 0xC6; break; // Ã
        case 0xC388: mapped = 0xD4; break; // È
        case 0xC389: mapped = 0xD5; break; // É
        case 0xC38A: mapped = 0xD9; break; // Ê
        case 0xC38C: mapped = 0xDF; break; // Ì
        case 0xC38D: mapped = 0xE0; break; // Í
        case 0xC392: mapped = 0xE4; break; // Ò
        case 0xC393: mapped = 0xE5; break; // Ó
        case 0xC394: mapped = 0xE9; break; // Ô
        case 0xC395: mapped = 0xE7; break; // Õ
        case 0xC399: mapped = 0xF4; break; // Ù
        case 0xC39A: mapped = 0xF5; break; // Ú
        case 0xC39D: mapped = 0xFF; break; // Ý
        case 0xC3A0: mapped = 0x80; break; // à
        case 0xC3A1: mapped = 0x81; break; // á
        case 0xC3A2: mapped = 0x8B; break; // â
        case 0xC3A3: mapped = 0x83; break; // ã
        case 0xC3A8: mapped = 0x92; break; // è
        case 0xC3A9: mapped = 0x93; break; // é
        case 0xC3AA: mapped = 0x97; break; // ê
        case 0xC3AC: mapped = 0x9D; break; // ì
        case 0xC3AD: mapped = 0x9E; break; // í
        case 0xC3B2: mapped = 0xA2; break; // ò
        case 0xC3B3: mapped = 0xA3; break; // ó
        case 0xC3B4: mapped = 0xA7; break; // ô
        case 0xC3B5: mapped = 0xA5; break; // õ
        case 0xC3B9: mapped = 0xB3; break; // ù
        case 0xC3BA: mapped = 0xB4; break; // ú
        case 0xC3BD: mapped = 0xBF; break; // ý
        case 0xC482: mapped = 0xC8; break; // Ă
        case 0xC483: mapped = 0x85; break; // ă
        case 0xC490: mapped = 0xD3; break; // Đ
        case 0xC491: mapped = 0x91; break; // đ
        case 0xC4A8: mapped = 0xE2; break; // Ĩ
        case 0xC4A9: mapped = 0xA0; break; // ĩ
        case 0xC5A8: mapped = 0xF7; break; // Ũ
        case 0xC5A9: mapped = 0xB6; break; // ũ
        case 0xC6A0: mapped = 0xEF; break; // Ơ
        case 0xC6A1: mapped = 0xAD; break; // ơ
        case 0xC6AF: mapped = 0xF9; break; // Ư
        case 0xC6B0: mapped = 0xB8; break; // ư
        default: break;
      }
      if (mapped) target += (char)mapped;
    } else if ((c1 & 0xF0) == 0xE0 && (i + 1) < len) { // 3-byte UTF-8
      uint8_t c2 = (uint8_t)source[i++];
      uint8_t c3 = (uint8_t)source[i++];
      uint32_t code = ((uint32_t)c1 << 16) | ((uint32_t)c2 << 8) | c3;
      uint8_t mapped = 0;
      switch (code) {
        case 0xE1BAA0: mapped = 0xC7; break; // Ạ
        case 0xE1BAA1: mapped = 0x84; break; // ạ
        case 0xE1BAA2: mapped = 0xC5; break; // Ả
        case 0xE1BAA3: mapped = 0x82; break; // ả
        case 0xE1BAA4: mapped = 0xCF; break; // Ấ
        case 0xE1BAA5: mapped = 0x8D; break; // ấ
        case 0xE1BAA6: mapped = 0xCE; break; // Ầ
        case 0xE1BAA7: mapped = 0x8C; break; // ầ
        case 0xE1BAA8: mapped = 0xD0; break; // Ẩ
        case 0xE1BAA9: mapped = 0x8E; break; // ẩ
        case 0xE1BAAA: mapped = 0xD1; break; // Ẫ
        case 0xE1BAAB: mapped = 0x8F; break; // ẫ
        case 0xE1BAAC: mapped = 0xD2; break; // Ậ
        case 0xE1BAAD: mapped = 0x90; break; // ậ
        case 0xE1BAAE: mapped = 0xCA; break; // Ắ
        case 0xE1BAAF: mapped = 0x87; break; // ắ
        case 0xE1BAB0: mapped = 0xC9; break; // Ằ
        case 0xE1BAB1: mapped = 0x86; break; // ằ
        case 0xE1BAB3: mapped = 0x88; break; // ẳ
        case 0xE1BAB4: mapped = 0xCB; break; // Ẵ
        case 0xE1BAB5: mapped = 0x89; break; // ẵ
        case 0xE1BAB6: mapped = 0xCC; break; // Ặ
        case 0xE1BAB7: mapped = 0x8A; break; // ặ
        case 0xE1BAB8: mapped = 0xD8; break; // Ẹ
        case 0xE1BAB9: mapped = 0x96; break; // ẹ
        case 0xE1BABA: mapped = 0xD6; break; // Ẻ
        case 0xE1BABB: mapped = 0x94; break; // ẻ
        case 0xE1BABC: mapped = 0xD7; break; // Ẽ
        case 0xE1BABD: mapped = 0x95; break; // ẽ
        case 0xE1BABE: mapped = 0xDB; break; // Ế
        case 0xE1BABF: mapped = 0x99; break; // ế
        case 0xE1BB80: mapped = 0xDA; break; // Ề
        case 0xE1BB81: mapped = 0x98; break; // ề
        case 0xE1BB82: mapped = 0xDC; break; // Ể
        case 0xE1BB83: mapped = 0x9A; break; // ể
        case 0xE1BB84: mapped = 0xDD; break; // Ễ
        case 0xE1BB85: mapped = 0x9B; break; // ễ
        case 0xE1BB86: mapped = 0xDE; break; // Ệ
        case 0xE1BB87: mapped = 0x9C; break; // ệ
        case 0xE1BB88: mapped = 0xE1; break; // Ỉ
        case 0xE1BB89: mapped = 0x9F; break; // ỉ
        case 0xE1BB8A: mapped = 0xE3; break; // Ị
        case 0xE1BB8B: mapped = 0xA1; break; // ị
        case 0xE1BB8C: mapped = 0xE8; break; // Ọ
        case 0xE1BB8D: mapped = 0xA6; break; // ọ
        case 0xE1BB8E: mapped = 0xE6; break; // Ỏ
        case 0xE1BB8F: mapped = 0xA4; break; // ỏ
        case 0xE1BB90: mapped = 0xEB; break; // Ố
        case 0xE1BB91: mapped = 0xA9; break; // ố
        case 0xE1BB92: mapped = 0xEA; break; // Ồ
        case 0xE1BB93: mapped = 0xA8; break; // ồ
        case 0xE1BB94: mapped = 0xEC; break; // Ổ
        case 0xE1BB95: mapped = 0xAA; break; // ổ
        case 0xE1BB96: mapped = 0xED; break; // Ỗ
        case 0xE1BB97: mapped = 0xAB; break; // ỗ
        case 0xE1BB98: mapped = 0xEE; break; // Ộ
        case 0xE1BB99: mapped = 0xAC; break; // ộ
        case 0xE1BB9A: mapped = 0xF1; break; // Ớ
        case 0xE1BB9B: mapped = 0xAF; break; // ớ
        case 0xE1BB9C: mapped = 0xF0; break; // Ờ
        case 0xE1BB9D: mapped = 0xAE; break; // ờ
        case 0xE1BB9E: mapped = 0xF2; break; // Ở
        case 0xE1BB9F: mapped = 0xB0; break; // ở
        case 0xE1BBA1: mapped = 0xB1; break; // ỡ
        case 0xE1BBA2: mapped = 0xF3; break; // Ợ
        case 0xE1BBA3: mapped = 0xB2; break; // ợ
        case 0xE1BBA4: mapped = 0xF8; break; // Ụ
        case 0xE1BBA5: mapped = 0xB7; break; // ụ
        case 0xE1BBA6: mapped = 0xF6; break; // Ủ
        case 0xE1BBA7: mapped = 0xB5; break; // ủ
        case 0xE1BBA8: mapped = 0xFB; break; // Ứ
        case 0xE1BBA9: mapped = 0xBA; break; // ứ
        case 0xE1BBAA: mapped = 0xFA; break; // Ừ
        case 0xE1BBAB: mapped = 0xB9; break; // ừ
        case 0xE1BBAC: mapped = 0xFC; break; // Ử
        case 0xE1BBAD: mapped = 0xBB; break; // ử
        case 0xE1BBAE: mapped = 0xFD; break; // Ữ
        case 0xE1BBAF: mapped = 0xBC; break; // ữ
        case 0xE1BBB0: mapped = 0xFE; break; // Ự
        case 0xE1BBB1: mapped = 0xBD; break; // ự
        case 0xE1BBB3: mapped = 0xBE; break; // ỳ
        case 0xE1BBB5: mapped = 0xC2; break; // ỵ
        case 0xE1BBB7: mapped = 0xC0; break; // ỷ
        case 0xE1BBB9: mapped = 0xC1; break; // ỹ
        default: break;
      }
      if (mapped) target += (char)mapped;
    }
  }
  return target;
}
