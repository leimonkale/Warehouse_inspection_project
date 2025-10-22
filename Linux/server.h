#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <mosquitto.h>
#include <dirent.h>
#include <sys/stat.h>
#include <arpa/inet.h>

// 原有配置（保留你的MQTT和录制相关）
#define MQTT_BROKER      "localhost"
#define MQTT_PORT        1883
#define MQTT_TOPIC       "sensor/data"
#define LOG_FILE         "mqtt_log.txt"
#define SAVE_DIR         "./rtsp_videos"
#define MAX_EVENTS       10
#define BUFFER_SIZE      1024

// 新增：TCP云盘配置
#define CLOUD_TCP_PORT   8889  // 云盘专用TCP端口
#define FILE_BUFFER_SIZE 4096  // 文件传输缓冲区

// 函数声明
int init_udp_server();                  // 保留你的UDP初始化（如果需要）
void *mqtt_thread(void *arg);           
void *record_thread(void *arg);         
void *cloud_thread(void *arg);          // 新增TCP云盘线程

#endif