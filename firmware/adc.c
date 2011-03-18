#include <stdio.h>
#include <stdint.h>

#include "adc.h"
#include "err.h"

#include <avr/io.h>

#include "uart.h"


const float VREF = 2.56;
const uint8_t mux_dual_illegalvalue = 0;

// one atom conversion
inline conv_result_t _single_conv(void) {
#if CONVERSION_BITS == 12
  ADCSRA |= (1<<ADSC);           // start
  while ( ADCSRA & (1<<ADSC) ) ; // just wait
  return ADCW;
#else
  ADCSRA |= (1<<ADSC);           // start
  while ( ADCSRA & (1<<ADSC) ) ; // just wait
  return ADCH;
#endif
}

inline uint8_t _mux_select(uint8_t ch1, uint8_t ch2, uint8_t gain) {

  uint8_t mux = 0;

  switch (gain) {
  case ADC_GAIN_1X:

    mux |= ch1;

    if ( ch2 == 1 ) {
    }
    else if ( ch2 == 2 ) {
      if (ch1>5)
	return mux_dual_illegalvalue;
      mux |= 1<<3;
    }
    else {
      return mux_dual_illegalvalue;
    }

    mux |= _BV(4);
    break;

  case ADC_GAIN_10X:
  case ADC_GAIN_200X:
  
    if (ch2 == 0) {
      if (ch1 == 0) 
	mux |= 0x00;
      else if (ch1 == 1)
	mux |= 0x01;
      else 
	return mux_dual_illegalvalue;
    }
    else if (ch2 == 2) {
      if (ch1 == 2) 
	mux |= 0x04;
      else if (ch1 == 1)
	mux |= 0x05;
      else 
	return mux_dual_illegalvalue;
    }
    else {
	return mux_dual_illegalvalue;
    }

    if ( gain == ADC_GAIN_200X )
      mux |= 1<<1;

    break;

  default:
    return mux_dual_illegalvalue;  // most illegal value!
  }

  return mux;
}

status_t adc_select_channel(uint8_t channel) {
  ADMUX &= 0xf8;             // null all current mux-bits
  ADMUX |= channel & 0x07;   // add the new mux-bits
  return NO_ERR;
}

status_t adc_select_channel_diff(uint8_t ch1, uint8_t ch2, uint8_t gain) {

  uint8_t mux = _mux_select(ch1, ch2, gain);

  if ( mux == mux_dual_illegalvalue )
    return ERR_ADC_ILLEGAL_CHANNEL;

  ADMUX &= 0xe0;
  ADMUX |= mux & 0x1f;

  return NO_ERR | mux;
}


status_t adc_init(void) {

  uint8_t conf = 0;

  conf |= 1 << ADPS0;     // .
  conf |= 0 << ADPS1;     // .
  conf |= 1 << ADPS2;     // prescale:32
  conf |= 0 << ADIE;      // interr. enable
  conf |= 0 << ADIF;      // interr. flag
  //  conf |= 0 << ADATE;     // auto trigger
  conf |= 0 << ADSC;      // start conv
  conf |= 0 << ADEN;      // enable

  ADCSRA = conf;

  conf = 0;
#if CONVERSION_BITS == 8
  conf |= 1 << ADLAR;     // left adjust
#else
  conf |= 0 << ADLAR;     // left adjust
#endif
  conf |= 1 << REFS0;     // .
  conf |= 0 << REFS1;     // internal 2.56V ref

  ADMUX = conf;

  return NO_ERR;
}

status_t adc_single_conversion(conv_result_t* retval) {

  uint8_t i;
  uint32_t result = 0;

  const uint8_t rounds = 4;

  ADCSRA |= 1 << ADEN;    // enable adc

  _single_conv();         // dummy conversion

  *retval = _single_conv();
 
  //  *retval = (uint16_t) (result/rounds);

  ADCSRA &= ~(0 << ADEN);    // disable adc

  return NO_ERR;
}

status_t adc_result2volt(uint16_t result, float* pVolt, float vref) {

  if (!pVolt)
    return ERR_NULLPOINTER;

  *pVolt = vref * result / 1024.0;

  return NO_ERR;
}

