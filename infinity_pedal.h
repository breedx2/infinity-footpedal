
#ifndef INFINITY_PEDAL_H
#define INFINITY_PEDAL_H 1

static t_class *infinity_pedal_class;
typedef struct _infinity_pedal {
	t_object pedal_object;

	t_outlet *pLeft;
	t_outlet *pMiddle;
	t_outlet *pRight;

} t_infinity_pedal;

typedef struct _thread_info {
	int readFd;
	int writeFd;

	int listenerCt;
	pthread_mutex_t mutex;
	t_infinity_pedal **listeners;

} t_thread_info;

bool open_pedal_and_start_thread(char *device_path);
void add_pedal_to_infos(t_infinity_pedal *pPedal);
void remove_pedal_from_infos(t_infinity_pedal *pPedal);
void output_buttons(t_infinity_pedal *pPedal, pedal_buttons buttons, pedal_buttons last_buttons);

#endif
