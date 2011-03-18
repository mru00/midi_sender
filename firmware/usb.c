#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <stdlib.h>

#include "types.h"
#include "usbdrv/usbdrv.h"
#include "read_signals.h"

static uchar replyBuffer[8];
static unsigned usbAddress;     /* current pointer for EEPROM address */
static unsigned usbWriteLen;


extern uchar conf_buffer[];

#define ledRedOn()    PORTC &= ~(1 << PC2)
#define ledRedOff()   PORTC |= (1 << PC2)


uchar usbFunctionSetup(uchar data[8]) {
  
  usbRequest_t    *rq = (void *)data;


  // load data
  if(rq->bRequest == 1){
	usbAddress = 1;
	usbWriteLen = rq->wLength.word;


	while (!eeprom_is_ready());
	eeprom_write_byte(conf_buffer, EEPROM_MAGIC);


	return 0xff;
  }


  if (rq->bRequest ==0 ) {
    ledRedOff();
  }

  return 0;
}


/* Writing to the EEPROM may take 7 * 8.5ms = 59.9ms which is slightly more
 * than the allowable <50ms. We usually get away with that.
 */
uchar   usbFunctionWrite(uchar *data, uchar len)
{

  if(usbWriteLen == 0)
	return 1;

  if(len > usbWriteLen)
	len = usbWriteLen;

  while (!eeprom_is_ready());
  eeprom_write_block(data, conf_buffer + usbAddress, len);

  
  usbAddress += len;
  usbWriteLen -= len;


  return usbWriteLen == 0;
}
