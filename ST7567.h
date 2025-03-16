#include <sys/unistd.h>
#include "esp32-hal.h"
#include <SPI.h>

#define PAGE_SIZE 8
#define XLevelH 0x10
#define YLevel 0xB0
#define Brightness 0x81
#define WIDTH 128
#define HEIGHT 32

#define LCD_CMD true
#define LCD_DATA false

int cs_pin;
int rst_pin;
int a0_pin;
int sck_pin;
int sda_pin;
int x_offset;

const unsigned char ST7567_F6x8[][8] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // sp
  0x10, 0x10, 0x10, 0x10, 0x00, 0x10, 0x00, 0x00,  // !
  0x28, 0x28, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00,  // "
  0x28, 0x28, 0x7C, 0x28, 0x7C, 0x28, 0x28, 0x00,  // #
  0x10, 0x3C, 0x50, 0x38, 0x14, 0x78, 0x10, 0x00,  // $
  0x00, 0x4C, 0x2C, 0x10, 0x08, 0x60, 0x60, 0x00,  // %
  0x30, 0x48, 0x50, 0x20, 0x54, 0x48, 0x34, 0x00,  // &
  0x30, 0x10, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,  // '
  0x08, 0x10, 0x20, 0x20, 0x20, 0x10, 0x08, 0x00,  // (
  0x20, 0x10, 0x08, 0x08, 0x08, 0x10, 0x20, 0x00,  // )
  0x00, 0x10, 0x54, 0x38, 0x54, 0x10, 0x00, 0x00,  // *
  0x00, 0x10, 0x10, 0x7C, 0x10, 0x10, 0x00, 0x00,  // +
  0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x08, 0x10,  // ,
  0x00, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00,  // -
  0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00,  // .
  0x00, 0x04, 0x08, 0x10, 0x20, 0x40, 0x00, 0x00,  // /
  0x38, 0x44, 0x4C, 0x54, 0x64, 0x44, 0x38, 0x00,  // 0
  0x10, 0x30, 0x10, 0x10, 0x10, 0x10, 0x38, 0x00,  // 1
  0x38, 0x44, 0x04, 0x08, 0x10, 0x20, 0x7C, 0x00,  // 2
  0x7C, 0x08, 0x10, 0x08, 0x04, 0x44, 0x38, 0x00,  // 3
  0x08, 0x18, 0x28, 0x48, 0x7C, 0x08, 0x08, 0x00,  // 4
  0x7C, 0x40, 0x78, 0x04, 0x04, 0x44, 0x38, 0x00,  // 5
  0x18, 0x20, 0x40, 0x78, 0x44, 0x44, 0x38, 0x00,  // 6
  0x7C, 0x04, 0x08, 0x10, 0x20, 0x20, 0x20, 0x00,  // 7
  0x38, 0x44, 0x44, 0x38, 0x44, 0x44, 0x38, 0x00,  // 8
  0x38, 0x44, 0x44, 0x3C, 0x04, 0x08, 0x30, 0x00,  // 9
  0x00, 0x30, 0x30, 0x00, 0x30, 0x30, 0x00, 0x00,  // :
  0x00, 0x30, 0x30, 0x00, 0x30, 0x10, 0x20, 0x00,  // ;
  0x08, 0x10, 0x20, 0x40, 0x20, 0x10, 0x08, 0x00,  // <
  0x00, 0x00, 0x7C, 0x00, 0x7C, 0x00, 0x00, 0x00,  // =
  0x20, 0x10, 0x08, 0x04, 0x08, 0x10, 0x20, 0x00,  // >
  0x38, 0x44, 0x04, 0x08, 0x10, 0x00, 0x10, 0x00,  // ?
  0x38, 0x44, 0x04, 0x34, 0x5C, 0x44, 0x38, 0x00,  // @
  0x10, 0x28, 0x44, 0x44, 0x7C, 0x44, 0x44, 0x00,  // A
  0x78, 0x44, 0x44, 0x78, 0x44, 0x44, 0x78, 0x00,  // B
  0x38, 0x44, 0x40, 0x40, 0x40, 0x44, 0x38, 0x00,  // C
  0x70, 0x48, 0x44, 0x44, 0x44, 0x48, 0x70, 0x00,  // D
  0x7C, 0x40, 0x40, 0x78, 0x40, 0x40, 0x7C, 0x00,  // E
  0x7C, 0x40, 0x40, 0x78, 0x40, 0x40, 0x40, 0x00,  // F
  0x38, 0x44, 0x40, 0x5C, 0x44, 0x44, 0x3C, 0x00,  // G
  0x44, 0x44, 0x44, 0x7C, 0x44, 0x44, 0x44, 0x00,  // H
  0x38, 0x10, 0x10, 0x10, 0x10, 0x10, 0x38, 0x00,  // I
  0x1C, 0x08, 0x08, 0x08, 0x08, 0x48, 0x30, 0x00,  // J
  0x44, 0x48, 0x50, 0x60, 0x50, 0x48, 0x44, 0x00,  // K
  0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x7C, 0x00,  // L
  0x44, 0x6C, 0x54, 0x54, 0x44, 0x44, 0x44, 0x00,  // M
  0x44, 0x44, 0x64, 0x54, 0x4C, 0x44, 0x44, 0x00,  // N
  0x38, 0x44, 0x44, 0x44, 0x44, 0x44, 0x38, 0x00,  // O
  0x78, 0x44, 0x44, 0x78, 0x40, 0x40, 0x40, 0x00,  // P
  0x38, 0x44, 0x44, 0x44, 0x54, 0x48, 0x34, 0x00,  // Q
  0x78, 0x44, 0x44, 0x78, 0x50, 0x48, 0x44, 0x00,  // R
  0x3C, 0x40, 0x40, 0x38, 0x04, 0x04, 0x78, 0x00,  // S
  0x7C, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00,  // T
  0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x38, 0x00,  // U
  0x44, 0x44, 0x44, 0x44, 0x44, 0x28, 0x10, 0x00,  // V
  0x44, 0x44, 0x44, 0x54, 0x54, 0x54, 0x28, 0x00,  // W
  0x44, 0x44, 0x28, 0x10, 0x28, 0x44, 0x44, 0x00,  // X
  0x44, 0x44, 0x44, 0x28, 0x10, 0x10, 0x10, 0x00,  // Y
  0x7C, 0x04, 0x08, 0x10, 0x20, 0x40, 0x7C, 0x00,  // Z
  0x38, 0x20, 0x20, 0x20, 0x20, 0x20, 0x38, 0x00,  // [
  0x00, 0x40, 0x20, 0x10, 0x08, 0x04, 0x00, 0x00,  // '\'
  0x38, 0x08, 0x08, 0x08, 0x08, 0x08, 0x38, 0x00,  // ]
  0x10, 0x28, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00,  // ^
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x00,  // _
  0x20, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,  // '
  0x00, 0x00, 0x38, 0x04, 0x3C, 0x44, 0x3C, 0x00,  // a
  0x40, 0x40, 0x58, 0x64, 0x44, 0x44, 0x78, 0x00,  // b
  0x00, 0x00, 0x38, 0x40, 0x40, 0x44, 0x38, 0x00,  // c
  0x04, 0x04, 0x34, 0x4C, 0x44, 0x44, 0x3C, 0x00,  // d
  0x00, 0x00, 0x38, 0x44, 0x7C, 0x40, 0x38, 0x00,  // e
  0x38, 0x24, 0x20, 0x70, 0x20, 0x20, 0x20, 0x00,  // f
  0x00, 0x00, 0x3C, 0x44, 0x44, 0x3C, 0x04, 0x38,  // g
  0x40, 0x40, 0x58, 0x64, 0x44, 0x44, 0x44, 0x00,  // h
  0x10, 0x00, 0x30, 0x10, 0x10, 0x10, 0x38, 0x00,  // i
  0x08, 0x00, 0x18, 0x08, 0x08, 0x08, 0x48, 0x30,  // j
  0x40, 0x40, 0x48, 0x50, 0x60, 0x50, 0x48, 0x00,  // k
  0x30, 0x10, 0x10, 0x10, 0x10, 0x10, 0x38, 0x00,  // l
  0x00, 0x00, 0x68, 0x54, 0x54, 0x44, 0x44, 0x00,  // m
  0x00, 0x00, 0x58, 0x64, 0x44, 0x44, 0x44, 0x00,  // n
  0x00, 0x00, 0x38, 0x44, 0x44, 0x44, 0x38, 0x00,  // o
  0x00, 0x00, 0x58, 0x64, 0x64, 0x58, 0x40, 0x40,  // p
  0x00, 0x00, 0x34, 0x4C, 0x4C, 0x34, 0x04, 0x04,  // q
  0x00, 0x00, 0x58, 0x64, 0x40, 0x40, 0x40, 0x00,  // r
  0x00, 0x00, 0x3C, 0x40, 0x38, 0x04, 0x78, 0x00,  // s
  0x20, 0x20, 0x70, 0x20, 0x20, 0x24, 0x18, 0x00,  // t
  0x00, 0x00, 0x44, 0x44, 0x44, 0x4C, 0x34, 0x00,  // u
  0x00, 0x00, 0x44, 0x44, 0x44, 0x28, 0x10, 0x00,  // v
  0x00, 0x00, 0x44, 0x44, 0x54, 0x54, 0x28, 0x00,  // w
  0x00, 0x00, 0x44, 0x28, 0x10, 0x28, 0x44, 0x00,  // x
  0x00, 0x00, 0x44, 0x44, 0x44, 0x3C, 0x04, 0x38,  // y
  0x00, 0x00, 0x7C, 0x08, 0x10, 0x20, 0x7C, 0x00,  // z
  0x10, 0x20, 0x20, 0x40, 0x20, 0x20, 0x10, 0x00,  // {
  0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00,  // |
  0x20, 0x10, 0x10, 0x08, 0x10, 0x10, 0x20, 0x00,  // }
  0x00, 0x00, 0x00, 0x20, 0x54, 0x08, 0x00, 0x00,  // ~
};

static unsigned char LCD_buffer[1024] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void ST7567_write_byte(uint8_t data, bool cmd) {
  if (cmd) {
    digitalWrite(a0_pin, LOW);
  } else {
    digitalWrite(a0_pin, HIGH);
  }
  digitalWrite(cs_pin, LOW);
  SPI.transfer(data);
  digitalWrite(cs_pin, HIGH);
}

void ST7567_pin_init() {
  pinMode(cs_pin, OUTPUT);
  pinMode(rst_pin, OUTPUT);
  pinMode(a0_pin, OUTPUT);
  pinMode(sck_pin, OUTPUT);
  pinMode(sda_pin, OUTPUT);

  digitalWrite(rst_pin, HIGH);
  digitalWrite(cs_pin, HIGH);
}

void display_inverse(int inverse) {
  if (inverse) {
    ST7567_write_byte(0xa7, LCD_CMD);
  } else {
    ST7567_write_byte(0xa6, LCD_CMD);
  }
}

void display_init(int _cs_pin, int _rst_pin, int _a0_pin, int _sck_pin, int _sda_pin, int _x_offset) {
  cs_pin = _cs_pin;
  rst_pin = _rst_pin;
  a0_pin = _a0_pin;
  sck_pin = _sck_pin;
  sda_pin = _sda_pin;
  x_offset = _x_offset;

  ST7567_pin_init();
  SPI.begin(sck_pin, -1, sda_pin, cs_pin);
  SPI.beginTransaction(SPISettings(800000, MSBFIRST, SPI_MODE0));
  ST7567_write_byte(0xe2, LCD_CMD);  //软复位
  delay(20);
  ST7567_write_byte(0xa0, LCD_CMD);        //列扫描顺序：从左到右
  ST7567_write_byte(0xc8, LCD_CMD);        //行扫描顺序：从下到上，c0从上到下
  ST7567_write_byte(0xa2, LCD_CMD);        //设置偏压比1/9
  ST7567_write_byte(0x2f, LCD_CMD);        //控制电源
  ST7567_write_byte(0x15, LCD_CMD);        //粗调对比度
  ST7567_write_byte(Brightness, LCD_CMD);  //微调对比度,进入微调对比度命令
  ST7567_write_byte(0x00, LCD_CMD);
  ST7567_write_byte(0x40, LCD_CMD);  //起始行：第一行开始

  // ST7567_write_byte(0X8D, LCD_CMD);  //SET DCDC命令
  // ST7567_write_byte(0X14, LCD_CMD);  //DCDC ON
  ST7567_write_byte(0xaf, LCD_CMD);  //开显示
}

void ST7567_set_pixel(unsigned char x, unsigned char y, bool color) {
  if (color) {
    LCD_buffer[(y / PAGE_SIZE) * WIDTH + x + x_offset] |= (1 << (y % PAGE_SIZE)) & 0xff;
  } else {
    LCD_buffer[(y / PAGE_SIZE) * WIDTH + x + x_offset] &= ~((1 << (y % PAGE_SIZE)) & 0xff);
  }
}

void ST7567_show_char(int x, int y, char chr) {
  unsigned char c = 0, i = 0, tmp, j = 0;
  c = chr - ' ';  //得到偏移后的值
  if (x > WIDTH - 1) {
    x = 0;
    y = y + 2;
  }
  for (i = 0; i < 8; i++) {
    tmp = ST7567_F6x8[c][i];
    for (j = 0; j < 8; j++) {
      if (tmp & (0x80 >> j)) {
        ST7567_set_pixel(x + j, y + i, 1);
      } else {
        ST7567_set_pixel(x + j, y + i, 0);
      }
    }
  }
}

void ST7567_display() {
  uint8_t i, n;
  for (i = 0; i < PAGE_SIZE; i++) {
    ST7567_write_byte(YLevel + i, LCD_CMD);  //设置页地址（0~7）
    for (n = 0; n < WIDTH; n++) {
      ST7567_write_byte(XLevelH + n, LCD_CMD);
      ST7567_write_byte(LCD_buffer[i * WIDTH + n], LCD_DATA);
    }
  }
  ST7567_write_byte(Brightness, LCD_CMD);  //微调对比度,进入微调对比度命令
  ST7567_write_byte(0x00, LCD_CMD);
}

void draw_string(int x, int y, String chr) {
  unsigned char j = 0, csize = 6;
  while (chr[j] != '\0') {
    ST7567_show_char(x, y, chr[j]);
    x += csize;
    if (x > 120) {
      x = 0;
      y += 8;
    }
    j++;
  }
  ST7567_display();
}

void display_rest() {
  memset(LCD_buffer, 0x00, 1024);
  ST7567_display();
}

void draw_pixel(int x, int y, bool color) {
  ST7567_set_pixel(x, y, color);
  ST7567_display();
}

void draw_fill_rect(int sx, int sy, int ex, int ey, int color) {
  int i, j;
  int width = ex - sx + 1;   //得到填充的宽度
  int height = ey - sy + 1;  //高度
  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      ST7567_set_pixel(sx + j, sy + i, color);
    }
  }
  ST7567_display();
}

void draw_line(int x1, int y1, int x2, int y2) {
  int xerr = 0, yerr = 0, delta_x, delta_y, distance;
  int incx, incy, uRow, uCol;

  delta_x = x2 - x1;  //计算坐标增量
  delta_y = y2 - y1;
  uRow = x1;
  uCol = y1;
  if (delta_x > 0) incx = 1;        //设置单步方向
  else if (delta_x == 0) incx = 0;  //垂直线
  else {
    incx = -1;
    delta_x = -delta_x;
  }
  if (delta_y > 0) incy = 1;
  else if (delta_y == 0) incy = 0;  //水平线
  else {
    incy = -1;
    delta_y = -delta_y;
  }
  if (delta_x > delta_y) distance = delta_x;  //选取基本增量坐标轴
  else distance = delta_y;
  for (int t = 0; t <= distance + 1; t++)  //画线输出
  {
    ST7567_set_pixel(uRow, uCol, 1);
    xerr += delta_x;
    yerr += delta_y;
    if (xerr > distance) {
      xerr -= distance;
      uRow += incx;
    }
    if (yerr > distance) {
      yerr -= distance;
      uCol += incy;
    }
  }
  ST7567_display();
}

void draw_rect(int x1, int y1, int x2, int y2) {
  draw_line(x1, y1, x2, y1);
  draw_line(x1, y1, x1, y2);
  draw_line(x1, y2, x2, y2);
  draw_line(x2, y1, x2, y2);
  ST7567_display();
}

void _draw_circle_8(int xc, int yc, int x, int y, bool color) {
  ST7567_set_pixel(xc + x, yc + y, color);
  ST7567_set_pixel(xc - x, yc + y, color);
  ST7567_set_pixel(xc + x, yc - y, color);
  ST7567_set_pixel(xc - x, yc - y, color);
  ST7567_set_pixel(xc + y, yc + x, color);
  ST7567_set_pixel(xc - y, yc + x, color);
  ST7567_set_pixel(xc + y, yc - x, color);
  ST7567_set_pixel(xc - y, yc - x, color);
}

void draw_circle(int xc, int yc, int r) {
  int x = 0, y = r, d;
  d = 3 - 2 * r;
  while (x <= y) {
    _draw_circle_8(xc, yc, x, y, true);
    if (d < 0) {
      d = d + 4 * x + 6;
    } else {
      d = d + 4 * (x - y) + 10;
      y--;
    }
    x++;
  }
  ST7567_display();
}

void draw_fill_circle(int xc, int yc, int r, bool color) {
  int x = 0, y = r, yi, d;
  d = 3 - 2 * r;
  while (x <= y) {
    for (yi = x; yi <= y; yi++) {
      _draw_circle_8(xc, yc, x, yi, color);
    }
    if (d < 0) {
      d = d + 4 * x + 6;
    } else {
      d = d + 4 * (x - y) + 10;
      y--;
    }
    x++;
  }
  ST7567_display();
}

void draw_bmp(int x, int y, int width, int height, const unsigned char BMP[], int mode) {
  int i, j, k;
  int tmp;
  for (i = 0; i < height; i++) {
    for (j = 0; j < (width + 7) / 8; j++) {
      if (mode) {
        tmp = BMP[i * ((width + 7) / 8) + j];
      } else {
        tmp = ~BMP[i * ((width + 7) / 8) + j];
      }
      for (k = 0; k < 8; k++) {
        if (tmp & (0x80 >> k)) {
          ST7567_set_pixel(x + j * 8 + k, y + i, 1);
        } else {
          ST7567_set_pixel(x + j * 8 + k, y + i, 0);
        }
      }
    }
  }
  ST7567_display();
}

void ST7567_set_memory(int x, int y, int width, int height, uint8_t* data, int len) {
  int _i_offset = 0, _j_offset = 0;
  for (int i = 0; i != len; i++) {
    for (int j = 0; j != 8; j++) {
      if (_j_offset == width) {
        _i_offset++;
        _j_offset = 0;
      }
      ST7567_set_pixel(x + _j_offset, y + _i_offset, ((data[i] << j) & 0x80) == 0x80);
      _j_offset++;
    }
  }
  ST7567_display();
}
