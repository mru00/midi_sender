
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>


#include "types.h"
#include "usbdrv/usbdrv.h"
#include "clock.h"
#include "myuart.h"
#include "midi.h"
#include "adc.h"
#include "read_signals.h"



#define ledRedOn()    PORTC &= ~(1 << PC2)
#define ledRedOff()   PORTC |= (1 << PC2)

void set_digital_out(uchar_t sig_num, uchar_t val);


void init_ports(void) {

  DDRB = 0;
  DDRC  = 0xff;
  PORTC = 0x00;
  PORTD = 0;
  PORTB = 0;		/* no pullups on USB and ISP pins */
  DDRD = ~(1 << PD2);	/* all outputs except PD2 = INT0 */
}

//#define UART_DBG

int main(void)
{
  uchar_t   i, j, k, cadc, wait = 10;
  uint8_t adc;
  uchar_t status; // used to set led


  init_ports();
  uart_init(MYUBRR(31250));
  adc_init();

  j = 0;
  while(--j){           /* USB Reset by device only required on Watchdog Reset */
      i = 0;
      while(--i);       /* delay >10ms for USB reset */
  }

  clockInit();          /* init timer */

  usbInit();
  sei();

  usbPoll();	
  
  read_signals();
	
  status = 0;
	
  j = wait;
  while(1) {


	usbPoll();	
	k--;

	if ( k-- == 0 ) {
	  j--;
	  //	  clockWait(200); 

	  if ( j == 0 ) {
		if ( status != 0 ) status = 0;
		else status = 0xff;
		j = wait;
	  }
	}


	for ( i = 0; i< 8; i++ ) {

#define CONF configuration[0][i]

	  switch ( CONF.si ) {
		  
		// unused:
	  case 0x0:
		break;

		// control analog
	  case 2:
		adc_select_channel(CONF.control.adc_sel);
		adc_single_conversion(&cadc);

		cadc >>= 1;

		if ( cadc != CONF.control.last_adc ) {
		  midi_send_control(3, CONF.control.midi_ch);
		  midi_send_data(CONF.control.midi_id);
		  midi_send_data((uchar_t)cadc);

		  CONF.control.last_adc = cadc;
		  wait = cadc+1;
		  j = wait;
		}
		  
		break;

		// control digital
	  case 1:
		// not impl
		break;

		// status
	  case 3:
		set_digital_out(i, status);
		break;

	  default:
		// panic!!
		break;

	  }
	}

  }
  return 0;
}

#define SET_UNSET(VAL, PORT, PIN) {if (val) { PORT |= (1<<PIN); } else { PORT &= ~(1<<PIN); } }

void set_digital_out(uchar_t sig_num, uchar_t val) {

  switch ( sig_num ) {
  case 0:
	SET_UNSET(val, PORTC, PC3);
	break;
  case 1:
	SET_UNSET(val, PORTC, PC2);
	break;
  case 2:
	SET_UNSET(val, PORTC, PC1);
	break;
  case 3:
	SET_UNSET(val, PORTC, PC0);
	break;
  case 4:
	SET_UNSET(val, PORTB, PB5);
	break;
  case 5:
	SET_UNSET(val, PORTB, PB4);
	break;
  case 6:
	SET_UNSET(val, PORTB, PB3);
	break;
  case 7:
	SET_UNSET(val, PORTB, PB2);
	break;
  default:
	// panic!
	break;
  }
}
