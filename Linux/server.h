#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <mosquitto.h> 

// 配置参数
#define MAX_EVENTS 10
#define UDP_PORT 8888
#define MQTT_BROKER "broker.emqx.io"
#define MQTT_PORT 1883
#define MQTT_TOPIC "mochen9227"
#define LOG_FILE "mqtt_log.txt"

// UDP服务器函数
int init_udp_server();

// MQTT相关函数
void init_mqtt_client(struct mosquitto **mosq);
void *mqtt_thread(void *arg);

#endif