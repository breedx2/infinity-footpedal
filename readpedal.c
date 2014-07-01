
#include <stdio.h>
#include "pedal.h"

int main(int argc, char **argv){

	if(argc < 2){
		printf("Usage: %s <device>\n", argv[0]);
		return 1;
	}

	if(!pedal_open(argv[1])){
		printf("Error opening file");
		return 1;
	}
	printf("Listening to pedal.  Stomp some buttons...\n");

	while(1){
		pedal_buttons buttons;
		if(!pedal_read(&buttons)){
			printf("Error reading pedal.\n");
			break;
		}
		printf("%d %d %d\n", buttons.left, buttons.middle, buttons.right);
	}

	pedal_close();
	return 0;
}
