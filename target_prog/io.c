#include "hal.h"
#include <stdint.h>
#include <stdlib.h>
#include "io.h"

#ifdef STM32F3
#include "stm32f3/stm32f3xx_hal_flash.h"
#endif
#ifdef STM32F1
#include "stm32f1/stm32f1xx_hal_flash.h"
#endif

#define UART_FRAME_LEN 64
void write_string(char *c)
{
  do {
    putch(*c);

  } while (*++c);
}


void read_string(char *buf, int len)
{
  for(int i = 0; i < len; i++) {
    while (buf[i] = getch(), buf[i] == '\0');

    if (buf[i] == '\n') {
      buf[i] = '\0';
      return;
    }
  }
  buf[len - 1] = '\0';
}

void write_buffer(char *buf, int len)
{
	int i,j;
	for(i = 0; i < len; i+=UART_FRAME_LEN) {
		if(i+UART_FRAME_LEN > len) {
			for(j = 0; i+j < len; ++j) putch(buf[i+j]);
		}else{
			for(j = 0; j < UART_FRAME_LEN; ++j) putch(buf[i+j]);
			getch();
		}
	}
}


void read_buffer(char *buf, int len)
{
	int i,j;
	for(i = 0; i < len; i+=UART_FRAME_LEN) {
		if(i+UART_FRAME_LEN > len) {
			for(j = 0; i+j < len; ++j) buf[i+j] = getch();
		}else{
			for(j = 0; j < UART_FRAME_LEN; ++j) buf[i+j] = getch();
			putch('a');
		}
	}
}

