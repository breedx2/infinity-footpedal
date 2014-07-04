
#define _GNU_SOURCE 1/*needed for pipe2*/
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include "m_pd.h"
#include "pedal.h"
#include "infinity_pedal.h"

pthread_t thread;
t_thread_info thread_info;

void *reader_thread(void *ptr){

	pedal_buttons last_buttons;

	while(keep_running()){

		pedal_buttons buttons;

		:/post("before reading with timeout...");
		int rc = pedal_read_timeout(&buttons, 250);
		if(rc == -1){
			perror("Error reading pedal.\n");
			break;
		}
		if(rc == 0){
			continue;
		}

		for(int i=0; i < thread_info.listenerCt; i++){
			t_infinity_pedal *pPedal = thread_info.listeners[i];
			output_buttons(pPedal, buttons, last_buttons);
			memcpy(&last_buttons, &buttons, sizeof(last_buttons));
		}
	}

	post("Infinity pedal thread is terminating");
	return NULL;
}

bool keep_running(){
	int readFd = thread_info.readFd;
	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(readFd, &rfds);
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	int rc = select(readFd + 1, &rfds, NULL, NULL, &tv);
	if(rc < 0){
		perror("Infinity thread select failed.");
		return false;
	}
	return (rc == 0) || !FD_ISSET(readFd, &rfds);
}

void output_buttons(t_infinity_pedal *pPedal, pedal_buttons buttons, pedal_buttons last_buttons){
	if(buttons.left != last_buttons.left){
		outlet_float(pPedal->pLeft, buttons.left);
	}
	if(buttons.middle != last_buttons.middle){
		outlet_float(pPedal->pMiddle, buttons.middle);
	}
	if(buttons.right != last_buttons.right){
		outlet_float(pPedal->pRight, buttons.right);
	}
}

// only called from Pd thread
void *infinity_pedal_new(){
	t_infinity_pedal *pPedal = (t_infinity_pedal*)pd_new(infinity_pedal_class);
	pPedal->pLeft = outlet_new(&(pPedal->pedal_object), &s_float);
	pPedal->pMiddle = outlet_new(&(pPedal->pedal_object), &s_float);
	pPedal->pRight = outlet_new(&(pPedal->pedal_object), &s_float);

	/* NOTE: The current approach assumes a single pedal.  We should ideally
	   have our thread_info grok multiple devices, and the reader thread would
	   use the file descriptors for each.  Sadly, without C++ this becomes even much
	   more of a hassle.  If/when we decide to go that route, this is an entry point
	   (switching on the device name symbol). 
	*/

	//ASSUMPTION: listenerCt is only modified in this thread
	if(thread_info.listenerCt == 0){
		open_pedal_and_start_thread();
	}
	add_pedal_to_infos(pPedal);	//register this instance as a listener
	
	return pPedal;
}

void add_pedal_to_infos(t_infinity_pedal *pPedal){
	thread_info.listenerCt++;
	int newSize = thread_info.listenerCt * (sizeof(t_infinity_pedal*));
	thread_info.listeners = realloc(thread_info.listeners, newSize);
	thread_info.listeners[thread_info.listenerCt - 1] = pPedal;
	char buff[1024];
	sprintf(buff, "infinity_pedal instance added (%d total)", thread_info.listenerCt);
	post(buff);
}

bool open_pedal_and_start_thread(){
	post("Attempting to open infinity pedal...");
	if(!pedal_open()){
		perror("Failed to open infinity pedal device.");
		post("Failed to open infinity pedal device.");
		return false;
	}
	post("Inifinity footpedal device opened.");
	int pfd[2];
	pipe2(pfd, O_NONBLOCK);
	thread_info.readFd = pfd[0];
	thread_info.writeFd = pfd[1];
	pthread_create(&thread, NULL, reader_thread, (void*)NULL);
	return true;
}

//Only called from Pd thread
void infinity_pedal_free(t_infinity_pedal *pPedal){
	remove_pedal_from_infos(pPedal);
	if(thread_info.listenerCt == 0){
		post("No more infinity_pedal instances. Closing pedal.");
		pedal_close();
		post("Sending shutdown notice to infinity pedal thread.");
		char x = 'x';
		write(thread_info.writeFd, &x, 1);
		pthread_join(thread, NULL);
		close(thread_info.writeFd);
		post("Infinity pedal done. Buh-bye.");
	}
}

void remove_pedal_from_infos(t_infinity_pedal *pPedal){
	thread_info.listenerCt--;
	t_infinity_pedal **pNew = NULL;
	if(thread_info.listenerCt > 0){
		pNew = malloc(thread_info.listenerCt * sizeof(t_infinity_pedal*));
		int dest = 0;
		for(int src = 0; src < thread_info.listenerCt+1; src++){
			if(thread_info.listeners[src] != pPedal){
				pNew[dest++] = thread_info.listeners[src];
			}
		}
		char buff[1024];
		sprintf(buff, "infinity_pedal instance removed (%d remaining)", thread_info.listenerCt);
		post(buff);
	}
	free(thread_info.listeners);
	thread_info.listeners = pNew;
}

void infinity_pedal_setup(){
	infinity_pedal_class = class_new(gensym("infinity_pedal"), 
		(t_newmethod)infinity_pedal_new, 
		(t_method)infinity_pedal_free, 
		sizeof(t_infinity_pedal), 
		CLASS_DEFAULT, A_DEFSYMBOL, 0);
}
