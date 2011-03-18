
#include "types.h"

#include "midi.h"
#include "myuart.h"



int midi_send_control(uchar_t message, uchar_t channel) {

  uart_putc( 0x80 | (message & 0x7) << 4 | (channel & 0x0f) );

  return 0;
}

int midi_send_data(uchar_t data) {

  uart_putc( data & 0x7f );

  return 0;
}
