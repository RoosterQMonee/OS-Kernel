#ifndef _DATA_H
#define _DATA_H

#include <stdint.h>

#include "../display/io.h"


void cpuid(uint32_t value, uint32_t *eax, uint32_t *ebx, unsigned int *ecx, uint32_t *edx)
{
  uint32_t eaxres, ebxres, ecxres, edxres;
  
  asm("xorl\t%eax, %eax");
  asm("xorl\t%ebx, %ebx");
  asm("xorl\t%ecx, %ecx");
  asm("xorl\t%edx, %edx");
  asm("movl\t%0, %%eax":"=m"(value));
  asm("cpuid");
  asm("movl\t%%eax, %0" : "=m" (eaxres));
  asm("movl\t%%ebx, %0" : "=m" (ebxres));
  asm("movl\t%%ecx, %0" : "=m" (ecxres));
  asm("movl\t%%edx, %0" : "=m" (edxres));
  
  *eax = eaxres;
  *ebx = ebxres;
  *ecx = ecxres;
  *edx = edxres;    
}

void print_cpu_eax(uint32_t eax)
{
  uint32_t step_id, model, family_id, proc_type, ext_mod_id, ext_fam_id;
  step_id = model = family_id = proc_type = ext_mod_id = ext_fam_id = eax;

  step_id &= (2 << 3) - 1;	//bits 0-3
  model >>= 4;	//bits 4-7
  model &= (2 << 3) - 1;
  family_id >>= 8;	//bits 8-11
  family_id &= (2 << 3) - 1;
  proc_type >>= 12;	//bits 12-13
  proc_type &= (2 << 1) - 1;
  ext_mod_id >>= 16;	//bits 16-19
  ext_mod_id &= (2 << 3) - 1;
  ext_fam_id >>= 20;	//bits 20-27
  ext_fam_id &= (2 << 7) - 1;

  kprintstr("EAX :-");

  kprintstr("\n    Stepping ID: ");
  kprintint(step_id);

  kprintstr("\n    Model: ");
  kprintint(model);

  kprintstr("\n    Family ID: ");
  kprintint(family_id);

  kprintstr("\n    Processor Type: ");
  kprintint(proc_type);

  kprintstr("\n    Extended Model ID: ");
  kprintint(ext_mod_id);

  kprintstr("\n    Extended Family ID: ");
  kprintint(ext_fam_id);
}


void print_cpu_ebx(uint32_t ebx)
{
  uint32_t brand_index, cache_line_size, max_addr_id, init_apic_id;
  brand_index = cache_line_size = max_addr_id = init_apic_id = 0;
  char *bytes = (char*)&ebx;

  brand_index = bytes[0];	//bits 0-7
  cache_line_size = bytes[1];	//bits 8-15
  max_addr_id = bytes[2];	//bits 16-23
  init_apic_id = bytes[3];	//bits 24-31

  kprintstr("EBX :-");
  kprintstr("\n    Brand Index: ");
  kprintint(brand_index);
  kprintstr("\n    Cache Line Size: ");
  kprintint(cache_line_size);
  kprintstr("\n    tMax Addressable ID for Logical Processors: ");
  kprintint(max_addr_id);
  kprintstr("\n    Initial APIC ID: ");
  kprintint(init_apic_id);
}

void print_cpu_edx(uint32_t edx)
{
  kprintstr("EDX :-");
  kprintstr("\n    bit-31 [ ");
  kprintbin(edx);
  kprintstr(" ] bit-0");

  kprintstr("\n    Bit 0 : FPU-x87 FPU on Chip");
  kprintstr("\n    Bit 1 : VME-Virtual-8086 Mode Enhancement");
  kprintstr("\n    Bit 2 : DE-Debugging Extensions");
  kprintstr("\n    Bit 3 : PSE-Page Size Extensions");
  kprintstr("\n    Bit 4 : TSC-Time Stamp Counter");
  kprintstr("\n    Bit 5 : MSR-RDMSR and WRMSR Support");
  kprintstr("\n    Bit 6 : PAE-Physical Address Extensions");
}

#endif