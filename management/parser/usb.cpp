#include <iostream>
#include <usb.h>

#include "usb.hpp"

using namespace std;

#define IDENT_VENDOR_NUM        0x16c0
#define IDENT_VENDOR_STRING     "mru"
#define IDENT_PRODUCT_NUM       0x05dc
#define IDENT_PRODUCT_STRING    ""
#define USB_ERROR_NOTFOUND  1
#define USB_ERROR_ACCESS    2
#define USB_ERROR_IO        3



typedef unsigned char* uchar_t;

static int  usbGetStringAscii(usb_dev_handle *dev, int index, int langid, char *buf, int buflen)
{
  char    buffer[256];
  int     rval, i;

  if((rval = usb_control_msg(dev, USB_ENDPOINT_IN, USB_REQ_GET_DESCRIPTOR, (USB_DT_STRING << 8) + index, langid, buffer, sizeof(buffer), 1000)) < 0)
	return rval;
  if(buffer[1] != USB_DT_STRING)
	return 0;
  if((unsigned char)buffer[0] < rval)
	rval = (unsigned char)buffer[0];
  rval /= 2;
  /* lossy conversion to ISO Latin1 */
  for(i=1;i<rval;i++){
	if(i > buflen)  /* destination buffer overflow */
	  break;
	buf[i-1] = buffer[2 * i];
	if(buffer[2 * i + 1] != 0)  /* outside of ISO Latin1 range */
	  buf[i-1] = '?';
  }
  buf[i-1] = 0;
  return i-1;
}


static int usbOpenDevice(usb_dev_handle **device, int vendor, const char *vendorName, int product, const char *productName)
{
  struct usb_bus      *bus;
  struct usb_device   *dev;
  usb_dev_handle      *handle = NULL;
  int                 errorCode = USB_ERROR_NOTFOUND;
  
  usb_find_busses();
  usb_find_devices();
  for(bus=usb_get_busses(); bus; bus=bus->next) {
	for(dev=bus->devices; dev; dev=dev->next){

	  if(dev->descriptor.idVendor == vendor && dev->descriptor.idProduct == product){
		char    string[256];
		int     len;
		handle = usb_open(dev); /* we need to open the device in order to query strings */
		if(!handle){
		  errorCode = USB_ERROR_ACCESS;
		  fprintf(stderr, "Warning: cannot open USB device: %s\n", usb_strerror());
		  continue;
		}
		if(vendorName == NULL && productName == NULL){  /* name does not matter */
		  cout << "1"<<endl;
		  break;
		}
		/* now check whether the names match: */
		len = usbGetStringAscii(handle, dev->descriptor.iManufacturer, 0x0409, string, sizeof(string));
		if(len < 0){
		  errorCode = USB_ERROR_IO;
		  //		  fprintf(stderr, "Warning: cannot query manufacturer for device: %s\n", usb_strerror());
		}else{
		  errorCode = USB_ERROR_NOTFOUND;
		  fprintf(stderr, "seen device from vendor ->%s<-\n", string);
		  if(strcmp(string, vendorName) == 0){
			len = usbGetStringAscii(handle, dev->descriptor.iProduct, 0x0409, string, sizeof(string));
			if(len < 0){
			  errorCode = USB_ERROR_IO;
			  fprintf(stderr, "Warning: cannot query product for device: %s\n", usb_strerror());
			}else{
			  errorCode = USB_ERROR_NOTFOUND;
			  //			  fprintf(stderr, "seen product ->%s<-\n", string); 
			  if(strcmp(string, productName) == 0)
				break;
			}
		  }
		}
		usb_close(handle);
		handle = NULL;
	  }
	}
	if(handle)
	  break;
  }
  if(handle != NULL){
	errorCode = 0;
	*device = handle;
  }
  return errorCode;
}


extern int send_data(unsigned char* buffer, int len) {
  
  usb_dev_handle      *handle = NULL;
  int                 nBytes;
  
  static int          didUsbInit = 0;
  
  if(!didUsbInit){
	didUsbInit = 1;
	usb_init();
  }


  if(usbOpenDevice(&handle, IDENT_VENDOR_NUM, "mru", IDENT_PRODUCT_NUM, "Midisender") != 0){
	fprintf(stderr, "Could not find Midisender with vid=0x%x pid=0x%x\n", IDENT_VENDOR_NUM, IDENT_PRODUCT_NUM);
	return 1;
  }
  
  nBytes = usb_control_msg(handle, USB_TYPE_VENDOR | USB_RECIP_INTERFACE | USB_ENDPOINT_OUT, 1, 1, 1, (char *)buffer, len, 5000);
  
  cout << nBytes << " bytes written"<< endl;

  if(nBytes < 0) {
	cerr <<  "USB error: %s\n" << usb_strerror();
	return 1;
  }
  
  usb_close(handle);

  return 0;
}
