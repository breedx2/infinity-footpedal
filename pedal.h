
#ifndef PEDAL_H
#define PEDAL_H 1

#include <stdbool.h>

typedef struct _pedal_buttons {
	unsigned char left;
	unsigned char middle;
	unsigned char right;
} pedal_buttons;

bool pedal_open(const char *device_path);
bool pedal_read(pedal_buttons *pButtons);
int pedal_fileno();
void pedal_close();

#endif
