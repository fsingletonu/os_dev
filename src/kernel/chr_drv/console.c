#include <chr_drv/console.h>

void set_screen(uint32_t offset)
{
    sys_screen.srceen_pos = (uint32_t)MEM_BASE + offset * 2;
    out_byte(CRT_ADDR_REG, CRT_START_ADDR_H);
    out_byte(CRT_DATA_REG, (uint8_t)(offset >> 8));
    out_byte(CRT_ADDR_REG, CRT_START_ADDR_L);
    out_byte(CRT_DATA_REG, (uint8_t)(offset));
}

void get_screen()
{
    uint32_t offset = 0;
    out_byte(CRT_ADDR_REG, CRT_START_ADDR_H);
    offset |= (uint32_t)(in_byte(CRT_DATA_REG) << 8);
    out_byte(CRT_ADDR_REG, CRT_START_ADDR_L);
    offset |= (uint32_t)(in_byte(CRT_DATA_REG));
    sys_screen.srceen_pos = (uint32_t)MEM_BASE + offset * 2;
}

void set_cursor(uint8_t y, uint8_t x)
{
    sys_cursor.y = y;
    sys_cursor.x = x;
    uint32_t offset = y * 80 + x;
    sys_cursor.cursor_pos = (uint32_t)MEM_BASE + offset * 2;
    out_byte(CRT_ADDR_REG, CRT_CURSOR_H);
    out_byte(CRT_DATA_REG, (uint8_t)(offset >> 8));
    out_byte(CRT_ADDR_REG, CRT_CURSOR_L);
    out_byte(CRT_DATA_REG, (uint8_t)(offset));
}

void get_cursor()
{
    uint32_t offset = 0;
    out_byte(CRT_ADDR_REG, CRT_CURSOR_H);
    offset |= (uint32_t)(in_byte(CRT_DATA_REG) << 8);
    out_byte(CRT_ADDR_REG, CRT_CURSOR_L);
    offset |= (uint32_t)(in_byte(CRT_DATA_REG));
    sys_cursor.y = offset / 80;
    sys_cursor.x = offset % 80;
    sys_cursor.cursor_pos = (uint32_t)MEM_BASE + offset * 2;
}

void write_string(char *string, size_t length)
{
    get_cursor();
    uint16_t *vga_ptr = (uint16_t *)sys_cursor.cursor_pos;
    for (size_t i = 0; i < length; i++, string++)
    {
        switch (*string)
        {
        case NUL:
            *vga_ptr = (uint16_t)ERASE;
            vga_ptr++;
            break;

        case ESC:
            break;

        case BEL:
            break;

        case BS:
            break;

        case HT:
            for (size_t i = 0; i < 4; i++)
            {
                *vga_ptr = (uint16_t)ERASE;
                vga_ptr++;
            }
            break;

        case LF:
            break;

        case VT:
            break;

        case FF:
            break;

        case CR:
            break;

        case DEL:
            break;

        default:
            *vga_ptr = (uint16_t)((VGA_TEXT_COLOR(VGA_COLOR_BG_BLACK, VGA_COLOR_LIGHT_GRAY) << 8) | (uint16_t)*string);
            vga_ptr++;
            break;
        }
    }
}

void console_clear()
{
    set_cursor(0, 0);
    uint16_t *vga_ptr = (uint16_t *)MEM_BASE;
    for (; vga_ptr < (uint16_t *)MEM_END; vga_ptr++)
    {
        *vga_ptr = (uint16_t)ERASE;
    }
}

void console_init()
{
    console_clear();
}