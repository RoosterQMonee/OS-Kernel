#ifndef _IO_H
#define _IO_H

#include <string.h>

#include "../globals.h"
#include "vga.h"


int tfg;
int tbg;


void terminal_initialize(void) 
{
  tfg = VGA_COLOR_LIGHT_GREY;
  tbg = VGA_COLOR_BLACK;
  
  terminal_row = 0;
  terminal_column = 0;
  terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
  terminal_buffer = (uint16_t*) 0xB8000;

  VGA_WIDTH = 80;
  VGA_HEIGHT = 25;

  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t index = y * VGA_WIDTH + x;
      terminal_buffer[index] = vga_entry(' ', terminal_color);
    }
  }
}

void terminal_clearscreen()
{
  for (size_t y = 0; y < VGA_HEIGHT; y++) {
    for (size_t x = 0; x < VGA_WIDTH; x++) {
      const size_t index = y * VGA_WIDTH + x;
      
      terminal_buffer[index] = vga_entry(' ', terminal_color);
    }
  }

  terminal_row = 0;
  terminal_column = 0;
}

void console_gotoxy(uint16 x, uint16 y) {
    terminal_column = x;
    terminal_row = y;
}

void terminal_newline()
{
  terminal_row++;
  terminal_column = 0;

  // Check if the cursor is at the bottom of the screen
  if (terminal_row == VGA_HEIGHT)
    terminal_row = 0;
}


void terminal_setcolor(uint8_t color) 
{
  terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) 
{
  const size_t index = y * VGA_WIDTH + x;
  terminal_buffer[index] = vga_entry(c, color);
}


void terminal_putchar(char c) 
{
    // Check if the character is a newline
    if (c == '\n') {
        terminal_newline();
    }
    // Check if the character is a tab
    else if (c == '\t') {
        c = ' ';  // Replace tabs with spaces for simplicity
    }
    else {
        // If the cursor is at the bottom row, scroll up the content by one row
        if (terminal_row == VGA_HEIGHT - 1) {
            terminal_clearscreen();
          
            for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
                for (size_t x = 0; x < VGA_WIDTH; x++) {
                    const size_t src_index = (y + 1) * VGA_WIDTH + x;
                    const size_t dest_index = y * VGA_WIDTH + x;
                  
                    terminal_buffer[dest_index] = terminal_buffer[src_index];
                }
            }

            // Clear the last row
            size_t last_row_index = (VGA_HEIGHT - 1) * VGA_WIDTH;
            for (size_t x = 0; x < VGA_WIDTH; x++) {
                terminal_buffer[last_row_index + x] = vga_entry(' ', terminal_color);
            }

            // Move the cursor to the last row
            terminal_row = VGA_HEIGHT - 1;
            terminal_column = 0;
        }

        // Now, you can proceed with putting the character on the screen
        terminal_putentryat(c, terminal_color, terminal_column, terminal_row);

        if (++terminal_column == VGA_WIDTH) {
            terminal_column = 0;

            if (++terminal_row == VGA_HEIGHT - 1) {
                // If the cursor reaches the second-to-last row, move back to the last row
                terminal_row = VGA_HEIGHT - 1;
            }
        }
    }
}




void terminal_write(const char* data, size_t size) 
{
  for (size_t i = 0; i < size; i++)
    terminal_putchar(data[i]);
}

void terminal_writestring(const char* data) 
{
  terminal_write(data, strlen(data));
}

// simplified.

void kprint_newline()
{
  terminal_row++;
  terminal_column = 0;
}

void kprintstr(const char* data) 
{
  terminal_write(data, strlen(data));
}

void kprintint(int num)
{
  char str_num[digit_count(num)+1];
  itoa2(num, str_num);
  
  kprintstr(str_num);
}

void krmprev()
{
  terminal_column--;

  terminal_putchar(' ');

  terminal_column--;
}

void kprintbin(uint32_t num)
{
  char bin_arr[32];
  uint32_t index = 31;
  uint32_t i;
  
  while (num > 0)
  {
    if(num & 1)
    {
      bin_arr[index] = '1';
    }
    else
    {
      bin_arr[index] = '0';
    }
    index--;
    num >>= 1;
  }

  for(i = 0; i < 32; ++i){
    if(i <= index)
        terminal_putchar('0');
    else
      terminal_putchar(bin_arr[i]);
  }
}

void clear_screen()
{
  terminal_clearscreen();

  terminal_row = 0;
  terminal_column = 0;
}


void printf(const char *format, ...) {
    char **arg = (char **)&format;
    int c;
    char buf[32];

    arg++;

    memset(buf, 0, sizeof(buf));
  
    while ((c = *format++) != 0) {
        if (c != '%')
          terminal_putchar(c);
        else {
            char *p, *p2;
            int pad0 = 0, pad = 0;

            c = *format++;
            if (c == '0') {
                pad0 = 1;
                c = *format++;
            }

            if (c >= '0' && c <= '9') {
                pad = c - '0';
                c = *format++;
            }

            switch (c) {
                case 'd':
                case 'u':
                case 'x':
                    itoa(buf, c, *((int *)arg++));
                    p = buf;
                    goto string;
                    break;

                case 's':
                    p = *arg++;
                    if (!p)
                        p = "(null)";

                string:
                    for (p2 = p; *p2; p2++)
                        ;
                    for (; p2 < p + pad; p2++)
                        terminal_putchar(pad0 ? '0' : ' ');
                    while (*p)
                        terminal_putchar(*p++);
                    break;

                default:
                    terminal_putchar(*((int *)arg++));
                    break;
            }
        }
    }
}

#endif