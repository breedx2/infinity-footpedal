
#include <stdio.h>
#include "pedal.h"

int main(int argc, char **argv){

	if(!pedal_open()){
		printf("Error opening file");
		return 1;
	}
	printf("Pedal opened successfully.\n");
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
