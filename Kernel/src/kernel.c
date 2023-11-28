#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "kernel/hardware/ps2/keyboard.h"
#include "kernel/hardware/ps2/mouse.h"

#include "kernel/hardware/memory/paging.h"
#include "kernel/hardware/memory/pmm.h"

#include "kernel/hardware/timing/timer.h"
#include "kernel/hardware/timing/rtc.h"

#include "kernel/hardware/idt/idt.h"
#include "kernel/hardware/gdt/gdt.h"
#include "kernel/hardware/ide/ide.h"
#include "kernel/hardware/data.h"

#include "kernel/user/command.h"
#include "kernel/user/bios32.h"

#include "kernel/display/io.h"
#include "kernel/globals.h"
#include "kernel/kernel.h"

#include "kernel/kinclude/multiboot.h"

#include "kdata.h"
#include "kdefs.h"


/*

TODO:
  - fix paging (might be a compiler issue :O)
  - add a compiler / interpreter
  - add scrolling
  
*/

REGISTERS16 in = {0};
REGISTERS16 out = {0};


void prepare_system()
{
  terminal_initialize();
  printf("[ OK ] Loaded VGA\n");

  gdt_init();
  printf("[ OK ] Loaded GDT\n");

  idt_init();
  printf("[ OK ] Loaded IDT\n");

  bios32_init();
  //bios32_service(0x12, &in, &out);
  printf("[ OK ] Loaded BIOS32\n");
  
  ata_init();
  printf("[ OK ] Loaded ATA\n");

  read_rtc();
  printf("[ OK ] Loaded RTC\n");
  
  timer_init();
  printf("[ OK ] Loaded Timer\n");

  keyboard_init();
  printf("[ OK ] Loaded Keyboard\n");

  mouse_init();
  printf("[ OK ] Loaded Mouse\n");
  
  cpuid(0x01, &eax, &ebx, &ecx, &edx);
  printf("[ OK ] Loaded CPU Data\n");

  kprint_newline();
}


KERNEL_MEMORY_MAP g_kmap;

int get_kernel_memory_map(KERNEL_MEMORY_MAP *kmap, MULTIBOOT_INFO *mboot_info) {
    uint32 i;

    if (kmap == NULL) return -1;
  
    kmap->kernel.k_start_addr = (uint32)&__kernel_section_start;
    kmap->kernel.k_end_addr = (uint32)&__kernel_section_end;
    kmap->kernel.k_len = ((uint32)&__kernel_section_end - (uint32)&__kernel_section_start);

    kmap->kernel.text_start_addr = (uint32)&__kernel_text_section_start;
    kmap->kernel.text_end_addr = (uint32)&__kernel_text_section_end;
    kmap->kernel.text_len = ((uint32)&__kernel_text_section_end - (uint32)&__kernel_text_section_start);

    kmap->kernel.data_start_addr = (uint32)&__kernel_data_section_start;
    kmap->kernel.data_end_addr = (uint32)&__kernel_data_section_end; 
    kmap->kernel.data_len = ((uint32)&__kernel_data_section_end - (uint32)&__kernel_data_section_start);

    kmap->kernel.rodata_start_addr = (uint32)&__kernel_rodata_section_start;
    kmap->kernel.rodata_end_addr = (uint32)&__kernel_rodata_section_end;
    kmap->kernel.rodata_len = ((uint32)&__kernel_rodata_section_end - (uint32)&__kernel_rodata_section_start);

    kmap->kernel.bss_start_addr = (uint32)&__kernel_bss_section_start;
    kmap->kernel.bss_end_addr = (uint32)&__kernel_bss_section_end; 
    kmap->kernel.bss_len = ((uint32)&__kernel_bss_section_end - (uint32)&__kernel_bss_section_start);

    kmap->system.total_memory = mboot_info->mem_low + mboot_info->mem_high;

    for (i = 0; i < mboot_info->mmap_length; i += sizeof(MULTIBOOT_MEMORY_MAP)) {
        MULTIBOOT_MEMORY_MAP *mmap = (MULTIBOOT_MEMORY_MAP *)(mboot_info->mmap_addr + i);
        if (mmap->type != MULTIBOOT_MEMORY_AVAILABLE) continue;
        // make sure kernel is loaded at 0x100000 by bootloader(see linker.ld)
        if (mmap->addr_low == kmap->kernel.text_start_addr) {
            // set available memory starting from end of our kernel, leaving 1MB size for functions exceution
            kmap->available.start_addr = kmap->kernel.k_end_addr + 1024 * 1024;
            kmap->available.end_addr = mmap->addr_low + mmap->len_low;
            // get availabel memory in bytes
            kmap->available.size = kmap->available.end_addr - kmap->available.start_addr;
            return 0;
        }
    }

    return -1;
}


void add_timer_function(TIMER_FUNCTION function, uint32 timeout) {
    TIMER_FUNC_ARGS args = {0};
    args.timeout = timeout;
  
    timer_register_function(function, &args);
}


void display_kernel_memory_map(KERNEL_MEMORY_MAP *kmap) {
    printf("kernel:\n");
    printf("  kernel-start: 0x%x, kernel-end: 0x%x, TOTAL: %d bytes\n", 
            kmap->kernel.k_start_addr, kmap->kernel.k_end_addr, kmap->kernel.k_len);
    printf("  text-start: 0x%x, text-end: 0x%x, TOTAL: %d bytes\n", 
            kmap->kernel.text_start_addr, kmap->kernel.text_end_addr, kmap->kernel.text_len);
    printf("  data-start: 0x%x, data-end: 0x%x, TOTAL: %d bytes\n", 
            kmap->kernel.data_start_addr, kmap->kernel.data_end_addr, kmap->kernel.data_len);
    printf("  rodata-start: 0x%x, rodata-end: 0x%x, TOTAL: %d\n",
            kmap->kernel.rodata_start_addr, kmap->kernel.rodata_end_addr, kmap->kernel.rodata_len);
    printf("  bss-start: 0x%x, bss-end: 0x%x, TOTAL: %d\n",
            kmap->kernel.bss_start_addr, kmap->kernel.bss_end_addr, kmap->kernel.bss_len);

    printf("total_memory: %d KB\n", kmap->system.total_memory);
    printf("available:\n");
    printf("  start_adddr: 0x%x\n  end_addr: 0x%x\n  size: %d\n", 
            kmap->available.start_addr, kmap->available.end_addr, kmap->available.size);
}


// note: timed fuctions seem to be (ms * 2.5)?


void kernel_main(unsigned long magic, unsigned long addr) 
{
  MULTIBOOT_INFO *mboot_info;
  
  disable_cursor();
  prepare_system();

  printf("magic: 0x%x\n", magic);

  clear_screen();

  // Usermode (-ish)
  
  if (DISPLAY_DETAILS == 1)
  {
    printf(banner);
  
    if (USE_SECONDS == 1)
      add_timer_function(s_time_display, 250);
  
    else
      add_timer_function(time_display, 2000);
  }
  
  if(magic == MULTIBOOT_BOOTLOADER_MAGIC) {      
      mboot_info = (MULTIBOOT_INFO *)addr;
      memset(&g_kmap, 0, sizeof(KERNEL_MEMORY_MAP));
    
      if (get_kernel_memory_map(&g_kmap, mboot_info) < 0) {
          printf("error: failed to get kernel memory map\n");
      }
    
      pmm_init(g_kmap.available.start_addr, g_kmap.available.size);
      pmm_get_max_blocks();
    
      // initialize a region of memory of size (4096 * 10), 10 blocks memory
    
      pmm_init_region(g_kmap.available.start_addr, PMM_BLOCK_SIZE * 10);

      //printf("Initializing paging...\n");
      //paging_init();
  }
  else
  {
      printf("error: invalid multiboot magic number\n");
  }

  char buf[256] = {'\0'};
  
  // add_timer_function(DEBUG, 100);

  DRIVE = ata_get_drive_by_model("VMware Virtual IDE Hard Drive");
  
  while(1) {
    if (strlen(buf) == 0 && terminal_column == 0)
    {
      printf(terminal_input);
    }
    else if (g_ch > 0)
    {
      char chr = kb_getchar();
      
      if (chr == '\b')
      {
        if (strlen(buf) > 0)
        {
          krmprev();

          buf[strlen(buf) - 1] = '\0';
        } 
      }
      else if (chr == '\n')
      {
        command_list_index++;
        command_list[command_list_index] = (char *)&buf;
        
        kprint_newline();
        command(buf);
        
        memset(buf, 0, sizeof(buf));
        memset(buf, 0, sizeof(buf)); // needed two when testing? idk it broke, fix later :)
      }
      else
      {
        printf("%c", chr);

        char myCharString[2] = {chr, '\0'};

        strcat(buf, myCharString);
      }
    }
  }
}