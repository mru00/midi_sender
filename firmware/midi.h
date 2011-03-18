#pragma once


typedef struct {

  uchar_t control : 1;
  
  union {

    struct {
      uint8_t message : 3;
      uint8_t channel : 4;
    };

    uint8_t data : 7;

  };
} midi_message_t;



int midi_send_control(uchar_t message, uchar_t channel);
int midi_send_data(uchar_t data);

