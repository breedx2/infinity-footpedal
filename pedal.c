/*
 Infinity pedal code.  Ported/borrowed heavily from the Python http://code.google.com/p/footpedal/
*/
#include <stdio.h>
#include "pedal.h"

int MAGICAL_OFFSETS[] = {4, 12, 20};

FILE *in = NULL;

bool pedal_open(char *device_path){
	in = fopen(device_path, "r");
	if(in == NULL){
		perror("Error opening file");
		return false;
	}
	return true;
}

bool pedal_read(pedal_buttons *pButtons){
	unsigned char buff[24];
	int read = fread(buff, 1, 24, in);
	if(read != 24){
		printf("Unexpected packet.\n");
		return false;
	}
	pButtons->left = buff[MAGICAL_OFFSETS[0]];
	pButtons->middle = buff[MAGICAL_OFFSETS[1]];
	pButtons->right = buff[MAGICAL_OFFSETS[2]];
	return true;
}

void pedal_close(){
	if(in != NULL){
		fclose(in);
		in = NULL;
	}
}

int pedal_fileno(){
	return fileno(in);
}
