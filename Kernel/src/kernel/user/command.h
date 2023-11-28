#include <stdbool.h>
#include <string.h>

#include "../hardware/data.h"
#include "../hardware/comms.h"
#include "../display/io.h"
#include "../globals.h"


#define CMD_LIST_LENGTH 13


char* commands[32] = {
  "help    ",
  "echo    ",
  "reboot  ",
  "clear   ",
  "cpueax  ",
  "cpuebx  ",
  "cpuedx  ",
  "drives  ",
  "termfg  ",
  "termbg  ",
  "setdrive",
  "drvread ",
  "drvwrite"
};

char* command_info[32] = {
  "Displays this help message",
  "Repeats input",
  "Restarts the system",
  "Clears the screen",
  "Displays all CPU data from EAX",
  "Displays all CPU data from EBX",
  "Displays all CPU data from EDX",
  "List all connected drive models",
  "Set terminal foreground (0-15)",
  "Set terminal background (0-15)",
  "Set the current drive to interact with",
  "Read a string from ATA disk drive (input = LBA)",
  "Write a string to ATA disk drive (input = str, LBA)"
};

int command_list_index = 0;
char* command_list[256] = {"help"};

int DRIVE;


const uint32 LBA = 0;
const uint8 NO_OF_SECTORS = 1;
char buf2[ATA_SECTOR_SIZE] = {0};



bool check_prefix(char* cmd, char* pref)
{
  for (int i = 0; i < strlen(pref); i++)
  {
    if (!(cmd[i] == pref[i]))
      return false;
  }

  return true;
}

void command(char* cmd)
{
  if (check_prefix(cmd, "help"))
  {
    for (int i = 0; i < CMD_LIST_LENGTH; i++)
      printf("%s : %s\n", commands[i], command_info[i]);

    return;
  }
  
  if (check_prefix(cmd, "clear"))
  {
    clear_screen();
    kprint_newline();

    return;
  }
  
  if (check_prefix(cmd, "echo"))
  {
    printf("%s\n", cmd + 5);

    return;
  }

  if (check_prefix(cmd, "cpueax"))
  {
    print_cpu_eax(eax);
    kprint_newline();

    return;
  }

  if (check_prefix(cmd, "cpuebx"))
  {
    print_cpu_ebx(ebx);
    kprint_newline();

    return;
  }

  if (check_prefix(cmd, "cpuedx"))
  {
    print_cpu_edx(edx);
    kprint_newline();

    return;
  }

  if (check_prefix(cmd, "reboot"))
  {
    printf("Restarting System...");

    uint8_t good = 0x02;
    while (good & 0x02)
        good = inb(0x64);
    
    outb(0x64, 0xFE);
    asm volatile("hlt");

    return;
  }

  if (check_prefix(cmd, "drvread"))
  {
    int lba = atoi(cmd + 8);

    memset(buf2, 0, sizeof(buf2));
    ide_read_sectors(DRIVE, NO_OF_SECTORS, lba, (uint32)buf2);
    printf("(Drive %d) Read: %s\n", DRIVE, buf2);

    terminal_color = vga_int_color(tfg, tbg);

    return;
  }

  if (check_prefix(cmd, "drvwrite"))
  {
    char* args[8];
    strsplit(args, cmd + 9, " ");

    strcpy(buf2, args[0]);
    ide_write_sectors(DRIVE, NO_OF_SECTORS, atoi(args[1]), (uint32)buf2);

    printf("(Drive %d) Write: %s\n", DRIVE, buf2);

    terminal_color = vga_int_color(tfg, tbg);

    return;
  }

  if (check_prefix(cmd, "drives"))
  {
    ata_list_drive_models();
    
    return;
  }

  if (check_prefix(cmd, "termfg"))
  {
    tfg = atoi(cmd + 7);

    terminal_color = vga_int_color(tfg, tbg);
    
    return;
  }

  if (check_prefix(cmd, "termbg"))
  {
    tbg = atoi(cmd + 7);
    
    terminal_color = vga_int_color(tfg, tbg);

    return;
  }

  if (check_prefix(cmd, "setdrive"))
  {
    int i;
    int changed_drive = 0;
    
    for(i = 0; i < MAXIMUM_IDE_DEVICES; i++) {
      if(strcmp((const char*)g_ide_devices[i].model, (char *)cmd + 9) == 0)
      {
        DRIVE = i;
        changed_drive = 1;

        printf("Set drive to: %d\n", i);
        
        break;
      }
    }

    if (changed_drive == 0)
      printf("Drive not found!\n");
    
    return;
  }
}
