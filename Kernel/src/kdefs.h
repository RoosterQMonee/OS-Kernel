#define USE_SECONDS 1
#define DISPLAY_DETAILS 1


void s_time_display(TIMER_FUNC_ARGS *args) {
  read_rtc();

  int px = terminal_column;
  int py = terminal_row;

  console_gotoxy(0, 0);

  printf(s_time_header, day, month, year, hour, minute, second);

  console_gotoxy(px, py);
}

void time_display(TIMER_FUNC_ARGS *args) {
  read_rtc();

  int px = terminal_column;
  int py = terminal_row;

  console_gotoxy(0, 0);

  printf(time_header, day, month, year, hour, minute);

  console_gotoxy(px, py);
}

void disable_cursor()  
{  
  outb(0x3D4, 0x0A);  
  outb(0x3D5, 0x20);  
}