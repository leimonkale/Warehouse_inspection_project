CC = gcc
CFLAGS = -Wall -I. $(shell pkg-config --cflags libmosquitto)
LDFLAGS = $(shell pkg-config --libs libmosquitto) -lpthread

all: server

# 注意：下面一行命令必须用 Tab 缩进（不能用空格）
server: main.o server_cloud.o server.o server_mqtt.o
	$(CC) -o $@ $^ $(LDFLAGS) 

main.o: main.c server.h
	$(CC) $(CFLAGS) -c $<  

server_cloud.o: server_cloud.c server.h
	$(CC) $(CFLAGS) -c $< 

server_mqtt.o: server_mqtt.c server.h
	$(CC) $(CFLAGS) -c $<  

server.o: server.c server.h
	$(CC) $(CFLAGS) -c $<  

clean:
	rm -f *.o server  

.PHONY: all clean