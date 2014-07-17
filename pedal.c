#include <stdio.h>
#include <stdlib.h>
#include "pedal.h"
#include <hidapi/hidapi.h>

const unsigned short PEDAL_VENDOR_ID = 0x05f3;
const unsigned short PEDAL_PRODUCT_ID = 0x00ff;

hid_device* device = NULL;

bool pedal_open(const char *device_path) {
	device = hid_open(PEDAL_VENDOR_ID, PEDAL_PRODUCT_ID, NULL);
	return device != NULL;
}

int pedal_read(pedal_buttons *pButtons) {
	return pedal_read_timeout(pButtons, -1);
}

int pedal_read_timeout(pedal_buttons *pButtons, int millis) {
	unsigned char cmdbuf[2] = { 0x01, 0x81 };
	unsigned char replybuf[2] = { 0x00, 0x00 };
	// Send an Output report to request the state (cmd 0x81)
	hid_write(device, cmdbuf, 2);
	// Read requested state
	int rc = hid_read_timeout(device, replybuf, 2, millis);
	if(rc < 2){
		return rc;
	}
	pButtons->left = replybuf[0] & 1;
	pButtons->middle = (replybuf[0] & 2) >> 1;
	pButtons->right = (replybuf[0] & 4) >> 2;
	return 1;
}

void pedal_close() {
	hid_close(device);
}
