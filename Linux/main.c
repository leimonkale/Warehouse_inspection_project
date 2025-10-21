#include "server.h"
#include <stdlib.h> 

void *record_thread(void *arg) {
    system("./record_mp4_loop.sh");
    return NULL;
}

int main() {
    pthread_t mqtt_tid; 
    pthread_t record_tid; 
    int epoll_fd, udp_fd;

    // 初始化UDP服务器
    udp_fd = init_udp_server();
    if (udp_fd < 0) {
        fprintf(stderr, "Failed to init UDP server\n");
        return -1;
    }

    // 初始化epoll
    epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        perror("epoll_create1");
        close(udp_fd);
        return -1;
    }

    // 添加UDP套接字到epoll
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = udp_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, udp_fd, &ev) < 0) {
        perror("epoll_ctl");
        close(epoll_fd);
        close(udp_fd);
        return -1;
    }

    // 启动MQTT线程
    if (pthread_create(&mqtt_tid, NULL, mqtt_thread, NULL) != 0) {
        perror("pthread_create mqtt");
        return -1;
    }

    if (pthread_create(&record_tid, NULL, record_thread, NULL) != 0) {
        perror("pthread_create record");
        return -1;
    }

    // epoll事件循环
    struct epoll_event events[MAX_EVENTS];
    while (1) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds < 0) {
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == udp_fd) {
                // 处理UDP客户端消息
                char buf[1024];
                struct sockaddr_in client_addr;
                socklen_t addr_len = sizeof(client_addr);
                ssize_t len = recvfrom(udp_fd, buf, sizeof(buf)-1, 0, 
                                      (struct sockaddr*)&client_addr, &addr_len);
                if (len < 0) {
                    perror("recvfrom");
                    continue;
                }
                buf[len] = '\0';
                printf("UDP Client %s:%d: %s\n", 
                       inet_ntoa(client_addr.sin_addr),
                       ntohs(client_addr.sin_port),
                       buf);
            }
        }
    }

    // 清理资源
    close(epoll_fd);
    close(udp_fd);
    pthread_join(mqtt_tid, NULL);
    pthread_join(record_tid, NULL);
    return 0;
}