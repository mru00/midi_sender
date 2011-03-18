
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <stdlib.h>


#include "types.h"
#include "read_signals.h"


signal_conf configuration[1][8];

extern uchar_t conf_buffer[];
uchar_t conf_buffer[30] EEMEM;


static uchar_t configure_as_control(uchar_t*, uchar_t, uchar_t);
static uchar_t configure_as_status(uchar_t*, uchar_t);

static uchar_t current_controller;
static uchar_t current_signal;

#define CONF configuration[current_controller][current_signal]


extern void read_signals(void) {

  uchar_t buffer[20];

  uchar_t size, pos = 0, pos1;
  uchar_t signal_type;
  uchar_t controller_size;

  uchar_t i;


  // mark unused configurations
  for ( i=0; i<8; i++ )
	configuration[0][i].si = 0;


  while ( !eeprom_is_ready() );
  eeprom_read_block(buffer, conf_buffer, 20);

  // is eeprom flashed?
  if ( buffer[pos++] != EEPROM_MAGIC )
	return;

  size = buffer[pos ++];

  while ( pos < size ) {

	current_controller = buffer[pos++];
	controller_size = buffer[pos++];

	pos1 = pos+controller_size;

	while ( pos < pos1 ) {
	  current_signal = buffer[pos++];
	  signal_type = buffer[pos++];

	  CONF.si = signal_type;
	  
	  switch ( signal_type ) {
	  case 1: // ST_CONTROL_DIGITAL
	  case 2: // ST_CONTROL_ANALOG
		pos = configure_as_control(buffer, pos, signal_type);
		break;
	  case 3: // ST_STATUS
		pos = configure_as_status(buffer, pos);
		break;
	  default:
		// panic!!!
		break;
	  }
	}
	  
  }
  

}



uchar_t configure_as_control(uchar_t* buffer, uchar_t pos, uchar_t signal_type) {

  CONF.control.midi_ch = buffer[pos++]; // channel
  CONF.control.midi_id = buffer[pos++]; // control number
  CONF.control.last_adc = 0;  // last adc value


  // this controller, configure input
  if ( current_controller == 0 ) {

	switch ( current_signal ) {
	case 0:
	  DDRC &= ~(1<<PC3); 
	  PORTC &= ~(1<<PC3);
	  if ( signal_type == 2 ) CONF.control.adc_sel = 3;
	  break;
	case 1:
	  DDRC &= ~(1<<PC2); 
	  PORTC &= ~(1<<PC2);
	  if ( signal_type == 2 ) CONF.control.adc_sel = 2;
	  break;
	case 2:
	  DDRC &= ~(1<<PC1); 
	  PORTC &= ~(1<<PC1);
	  if ( signal_type == 2 ) CONF.control.adc_sel = 1;
	  break;
	case 3:
	  DDRC &= ~(1<<PC0); 
	  PORTC &= ~(1<<PC0);
	  if ( signal_type == 2 ) CONF.control.adc_sel = 0;
	  break;
	case 4:
	  DDRB &= ~(1<<PB5); 
	  PORTB &= ~(1<<PB5);
	  CONF.control.adc_sel = 0xff;
	  break;
	case 5:
	  DDRB &= ~(1<<PB4); 
	  PORTB &= ~(1<<PB4);
	  CONF.control.adc_sel = 0xff;
	  break;
	case 6:
	  DDRB &= ~(1<<PB3); 
	  PORTB &= ~(1<<PB3);
	  CONF.control.adc_sel = 0xff;
	  break;
	case 7:
	  DDRB &= ~(1<<PB2); 
	  PORTB &= ~(1<<PB2);
	  CONF.control.adc_sel = 0xff;
	  break;
	default:
	  // panic!
	  break;
	}
  }
  else {
	// configure slave controllers??
  }

  
  return pos;

}


uchar_t configure_as_status(uchar_t* buffer, uchar_t pos) {


  // this controller, configure input
  if ( current_controller == 0 ) {

	switch ( current_signal ) {
	case 0:
	  DDRC |= (1<<PC3); 
	  break;
	case 1:
	  DDRC |= (1<<PC2); 
	  break;
	case 2:
	  DDRC |= (1<<PC1); 
	  break;
	case 3:
	  DDRC |= (1<<PC0); 
	  break;
	case 4:
	  DDRB |= (1<<PB5); 
	  break;
	case 5:
	  DDRB |= (1<<PB4); 
	  break;
	case 6:
	  DDRB |= (1<<PB3); 
	  break;
	case 7:
	  DDRB |= (1<<PB2); 
	  break;
	default:
	  // panic!
	  break;
	}
  }
  else {
	// configure slave controllers??
  }


  return pos;
}
