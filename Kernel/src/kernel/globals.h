#ifndef _GLOBALS_H
#define _GLOBALS_H

static size_t VGA_WIDTH;
static size_t VGA_HEIGHT;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;

uint32_t eax, ebx, ecx, edx;

#endif