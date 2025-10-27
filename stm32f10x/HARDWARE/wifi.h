#ifndef __WIFI_H
#define __WIFI_H	 

extern char received_msg[100];

void wifi_GPIO_Init(void);
void rst_wifi(void);
void init_wifi(void);
void send_data_mqtt(void);  //∑¢ÀÕmqtt–≈œ¢
int wifi_receive_mqtt_msg(char *msg_buf);

#endif
