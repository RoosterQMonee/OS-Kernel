#ifndef _PIT_H
#define _PIT_H

#include "comms.h"


unsigned read_pit_count(void) {
  unsigned count = 0;

  cli();

  outb(0x43,0b0000000);

  count = inb(0x40);
  count |= inb(0x40)<<8;

  return count;
}

void set_pit_count(unsigned count) {
  cli();

  outb(0x40,count&0xFF);
  outb(0x40,(count&0xFF00)>>8);
  
  return;
}

#endif