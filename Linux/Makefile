CC = gcc
CFLAGS = -Wall -I. $(shell pkg-config --cflags libmosquitto)
LDFLAGS = $(shell pkg-config --libs libmosquitto) -lpthread

all: server

server: main.o server.o server_mqtt.o
	$(CC) -o $@ $^ $(LDFLAGS)

main.o: main.c server.h
	$(CC) $(CFLAGS) -c $<

server.o: server.c server.h
	$(CC) $(CFLAGS) -c $<

server_mqtt.o: server_mqtt.c server.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o server