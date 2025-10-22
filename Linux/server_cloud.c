#include "server.h"

// 发送响应给TCP客户端
static void send_response(int client_fd, const char *msg) {
    if (msg) {
        send(client_fd, msg, strlen(msg), 0);
        send(client_fd, "\n", 1, 0);  // 换行符作为响应结束标记
    }
}

// 列出SAVE_DIR下的MP4文件
static void list_mp4_files(int client_fd) {
    DIR *dir = opendir(SAVE_DIR);
    if (!dir) {
        send_response(client_fd, "ERROR:  Failed to open directory");
        return;
    }

    struct dirent *entry;
    char file_list[4096] = "MP4_LIST:";
    size_t list_len = strlen(file_list);

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {  
            char *ext = strrchr(entry->d_name, '.');
            if (ext && strcasecmp(ext, ".mp4") == 0) {  // 匹配.mp4后缀
                if (list_len + strlen(entry->d_name) + 2 < sizeof(file_list)) {
                    list_len += snprintf(file_list + list_len,
                                        sizeof(file_list) - list_len,
                                        "%s,", entry->d_name);
                } else {
                    send_response(client_fd, "ERROR:  File list too long");
                    closedir(dir);
                    return;
                }
            }
        }
    }
    closedir(dir);

    if (list_len == strlen("MP4_LIST:")) {
        strcat(file_list, "File not found available MP4");
    }
    send_response(client_fd, file_list);
}

static void send_file_content(int client_fd, const char *file_path) {
    struct stat file_stat;
    if (stat(file_path, &file_stat) < 0) {
        send_response(client_fd, "ERROR: File not found");
        return;
    }

    FILE *file = fopen(file_path, "rb");
    if (!file) {
        send_response(client_fd, "ERROR: File disable to open");
        return;
    }

    const char *filename = strrchr(file_path, '/') ? strrchr(file_path, '/') + 1 : file_path;
    char header[BUFFER_SIZE];
    snprintf(header, BUFFER_SIZE, "FILE_HEADER:%s:%ld", filename, file_stat.st_size);
    send_response(client_fd, header);

    char ack[32] = {0};
    if (recv(client_fd, ack, sizeof(ack)-1, 0) <= 0 || strcmp(ack, "READY") != 0) {
        send_response(client_fd, "ERROR:  Client not ready");
        fclose(file);
        return;
    }

    char buffer[FILE_BUFFER_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (send(client_fd, buffer, bytes_read, 0) != (ssize_t)bytes_read) {
            send_response(client_fd, "ERROR: File send faile");
            fclose(file);
            return;
        }
    }

    send_response(client_fd, "FILE_FIN");
    recv(client_fd, ack, sizeof(ack)-1, 0);  
    fclose(file);
    printf("文件发送完成: %s\n", filename);
}

static void *handle_client(void *arg) {
    int client_fd = *(int*)arg;
    free(arg);  

    char buffer[BUFFER_SIZE];
    while (1) {
        
        ssize_t len = recv(client_fd, buffer, BUFFER_SIZE-1, 0);
        if (len <= 0) {
            break;  
        }
        buffer[len] = '\0';
        buffer[strcspn(buffer, "\r\n")] = '\0';  

        if (strcmp(buffer, "LIST_MP4") == 0) {
            list_mp4_files(client_fd);
        } else if (strncmp(buffer, "GET_MP4:", 8) == 0) {
            char file_path[BUFFER_SIZE];
            snprintf(file_path, BUFFER_SIZE, "%s/%s", SAVE_DIR, buffer + 8);
            send_file_content(client_fd, file_path);
        } else if (strcmp(buffer, "GET_LOG") == 0) {
            send_file_content(client_fd, LOG_FILE);
        } else if (strcmp(buffer, "QUIT") == 0) {
            send_response(client_fd, "BYE");
            break;  
        } else {
            send_response(client_fd, "ERROR: Unknown command (support: LIST_MP4/GET_MP4:/GET_LOG/QUIT)");
        }
    }

    close(client_fd);
    return NULL;
}

void *cloud_thread(void *arg) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("云盘TCP套接字创建失败");
        return NULL;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  
    server_addr.sin_port = htons(CLOUD_TCP_PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("云盘TCP端口绑定失败");
        close(server_fd);
        return NULL;
    }

    if (listen(server_fd, 10) < 0) {
        perror("云盘TCP监听失败");
        close(server_fd);
        return NULL;
    }

    printf("云盘TCP服务器启动，监听端口: %d\n", CLOUD_TCP_PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_fd < 0) {
            perror("接受客户端连接失败");
            continue;
        }

        printf("新云盘客户端连接: %s:%d\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        pthread_t client_tid;
        int *pfd = malloc(sizeof(int));
        *pfd = client_fd;
        if (pthread_create(&client_tid, NULL, handle_client, pfd) != 0) {
            perror("创建客户端处理线程失败");
            close(client_fd);
            free(pfd);
        } else {
            pthread_detach(client_tid);  
        }
    }

    close(server_fd);
    return NULL;
}