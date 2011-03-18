#pragma once


#define EEPROM_MAGIC 0xad


typedef struct {
  uchar_t si : 3;
  union {

	struct {
	  uchar_t midi_ch : 4;
	  uchar_t midi_id : 7;
	  uchar_t adc_sel : 3;
	  uchar_t last_adc : 7;
	} control;

  };

} signal_conf;


extern void read_signals(void);
//extern uchar_t configuration[1][8][5];

extern signal_conf configuration[1][8];
