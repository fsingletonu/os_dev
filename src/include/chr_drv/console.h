#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <chr_drv/io.h>

#define CRT_ADDR_REG 0x3d4
#define CRT_DATA_REG 0x3d5

#define CRT_START_ADDR_H 0xc
#define CRT_START_ADDR_L 0xd
#define CRT_CURSOR_H 0xe
#define CRT_CURSOR_L 0xf

#define MEM_BASE 0x000b8000
#define MEM_SIZE 0x4000
#define MEM_END (MEM_BASE + MEM_SIZE)
#define WIDTH 80
#define HEIGHT 25

#define VGA_COLOR_BLACK 0x00
#define VGA_COLOR_BLUE 0x01
#define VGA_COLOR_GREEN 0x02
#define VGA_COLOR_CYAN 0x03
#define VGA_COLOR_RED 0x04
#define VGA_COLOR_MAGENTA 0x05
#define VGA_COLOR_BROWN 0x06
#define VGA_COLOR_LIGHT_GRAY 0x07
#define VGA_COLOR_DARK_GRAY 0x08
#define VGA_COLOR_LIGHT_BLUE 0x09
#define VGA_COLOR_LIGHT_GREEN 0x0A
#define VGA_COLOR_LIGHT_CYAN 0x0B
#define VGA_COLOR_LIGHT_RED 0x0C
#define VGA_COLOR_LIGHT_MAGENTA 0x0D
#define VGA_COLOR_YELLOW 0x0E
#define VGA_COLOR_WHITE 0x0F

#define VGA_COLOR_BG_BLACK 0x00
#define VGA_COLOR_BG_BLUE 0x10
#define VGA_COLOR_BG_GREEN 0x20
#define VGA_COLOR_BG_CYAN 0x30
#define VGA_COLOR_BG_RED 0x40
#define VGA_COLOR_BG_MAGENTA 0x50
#define VGA_COLOR_BG_BROWN 0x60
#define VGA_COLOR_BG_LIGHT_GRAY 0x70

#define VGA_BLINK_ENABLE 0x80
#define VGA_BLINK_DISABLE 0x00

#define VGA_TEXT_COLOR(bc, fc) ((bc & 0xF0) | (fc & 0x0F))

#define NUL 0x00
#define ENQ 0x05
#define ESC 0x1b // ESC
#define BEL 0x07 // \a
#define BS 0x08  // \b
#define HT 0x09  // \t
#define LF 0x0a  // \n
#define VT 0x0b  // \v
#define FF 0x0c  // \f
#define CR 0x0d  // \r
#define DEL 0x7f

#define ERASE 0x0720

typedef struct cursor
{
    uint8_t x, y;
    uint32_t all;
    uint32_t cursor_pos;
} cursor;
cursor sys_cursor;

typedef struct screen
{
    uint32_t srceen_pos;
} screen;
screen sys_screen;

void set_screen(uint32_t offset);
void get_screen(void);
void set_cursor(uint8_t y, uint8_t x);
void get_cursor(void);
void write_string(char *string, size_t length);
void console_init(void);

#endif
