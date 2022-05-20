#include <DWT.h>
#include <hal.h>
#include <io.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "device/stm32f3xx.h"

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#ifndef CODE
	#define CODE 9999
#endif

uint32_t tries = 40;
char     InputCode[4] = { '0' };
char     Code[4]      = { STR(CODE) };
char     str[500];
uint32_t device_info[2];

extern uint32_t verify_code( char*, char* );

int main(void) {
  platform_init();
  init_uart();
  trigger_setup();
  while (1) {
    led_ok(0);
    char cmd = getch();
    led_ok(1);
    switch (cmd) {
    case 's': // status
      write_string("ok");
      break;
    case 'c':
      read_buffer(InputCode,4);
      if(tries > 0 ) {
        trigger_high();
        uint32_t res = verify_code(InputCode,Code);
        trigger_low();
        if(!res) {
          sprintf(str,"Success!");
          write_string(str);
        }else {
          tries = tries -1;
          sprintf(str,"Wrong code. Remaining tries: %ld",tries);
          write_string(str);
        }
      } else {
        sprintf(str,"Device locked");
        write_string(str);
      }
      break;
    }
  }
}
