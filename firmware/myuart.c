
#include <avr/io.h>
#include <avr/interrupt.h>

#include "types.h"
#include "myuart.h"


void uart_putc(uchar_t data) {

  // wait for empty transmit buffer
  while ( ! (UCSRA & ( 1 << UDRE) ) ) ;

  // put data into buffer, sends it
  UDR = data;
}



void uart_init(unsigned int baudrate) {




  //  UBRRH = (uchar_t)(baudrate>>8);
  //  UBRRL = (uchar_t) baudrate;


  UBRRH = (uchar_t)(USART_UBBR_VALUE>>8);
  UBRRL = (uchar_t) USART_UBBR_VALUE;

  // enable USART
  UCSRB = (1<<TXEN);
    
  // asynchronous, 8data, no parity, 1stop bit
  UCSRC = (1<<URSEL)|(1<<UCSZ0)|(1<<UCSZ1);
}
