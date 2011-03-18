// mru, 03.03.2007

#ifndef _ADC_H_
#define _ADC_H_

#include <stdint.h>
#include "err.h"


#define ADC_GAIN_1X   0
#define ADC_GAIN_10X  1
#define ADC_GAIN_200X 2


#define CONVERSION_BITS 8
//#define CONVERSION_BITS 12

#if CONVERSION_BITS == 8
typedef uint8_t conv_result_t;
#else
typedef uint16_t conv_result_t;
#endif 


extern status_t adc_init(void);

// selects the channel for a single ended conversion
// (absolute measurement against GND)
extern status_t adc_select_channel(uint8_t channel);

// selects the channels for a dual-ended conversion
// ( measures V{channel1}-V{channel2} )
extern status_t adc_select_channel_diff(uint8_t ch1, uint8_t ch2, uint8_t g);


extern status_t adc_single_conversion(conv_result_t* retval);

// only valid if internal vref=2.56 is selected
extern status_t adc_result2volt(uint16_t result, float* pVolt, float vref);

#endif
