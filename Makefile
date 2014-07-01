
CC = gcc
LD = ld
INCLUDES = -I/usr/local/include/
CFLAGS = -Wall -fPIC $(INCLUDES) -std=gnu99 
#LDFLAGS = -lstdc++ -lc -lm -export-dynamic -shared
LDFLAGS = -lm -export-dynamic -shared

all: readpedal infinity_pedal.pd_linux

readpedal: readpedal.c pedal.o
	$(CC) $(CFLAGS) -o $@ $< pedal.o

infinity_pedal.pd_linux: infinity_pedal.o pedal.o
	$(LD) $(LDFLAGS) -o $@ infinity_pedal.o pedal.o

infinity_pedal.o: infinity_pedal.c infinity_pedal.h
	$(CC) $(CFLAGS) -c $< 

pedal.o: pedal.c pedal.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm *.o readpedal infinity_pedal.pd_linux
