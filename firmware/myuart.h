#pragma once

#define MYUBRR(baud) ( (((F_OSC)/ (baud) )/16)-1)

#define USART_BAUD 31250ul

#define USART_UBBR_VALUE ((F_OSC/(USART_BAUD<<4))-1)



void uart_putc(uchar_t data);
void uart_init(unsigned int baudrate);
