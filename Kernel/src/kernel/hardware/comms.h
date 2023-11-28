#ifndef _COMMS_H
#define _COMMS_H

#include <stdint.h>


uint8_t inb(uint16_t port)
{
  uint8_t data;
  asm volatile("inb %1, %0" : "=a"(data) : "Nd"(port));
  return data;
}

void outb(uint16_t port, uint8_t data)
{
  asm volatile("outb %0, %1" : : "a"(data), "Nd"(port));
}


uint8_t inportb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void outportb(uint16_t port, uint8_t val) {
    asm volatile("outb %1, %0" :: "dN"(port), "a"(val));
}


uint16_t inports(uint16_t port) {
    uint16_t rv;
    asm volatile ("inw %1, %0" : "=a" (rv) : "dN" (port));
    return rv;
}

void outports(uint16_t port, uint16_t data) {
    asm volatile ("outw %1, %0" : : "dN" (port), "a" (data));
}


uint32_t inportl(uint16_t port) {
    uint32_t rv;
    asm volatile ("inl %%dx, %%eax" : "=a" (rv) : "dN" (port));
    return rv;
}

void outportl(uint16_t port, uint32_t data) {
    asm volatile ("outl %%eax, %%dx" : : "dN" (port), "a" (data));
}


void cli()
{
  asm volatile("cli");
}

void sti()
{
  asm volatile("sti");
}

#endif