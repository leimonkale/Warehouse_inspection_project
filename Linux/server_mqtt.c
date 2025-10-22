#include "server.h"

void mqtt_message_callback(struct mosquitto *mosq, void *userdata, 
                          const struct mosquitto_message *msg) {
    if (msg->payloadlen == 0) return;

    FILE *log_file = fopen(LOG_FILE, "a");
    if (log_file) {
        time_t now = time(NULL);
        fprintf(log_file, "[%s] Topic: %s, Message: %.*s\n",
                ctime(&now), msg->topic,
                (int)msg->payloadlen, (char*)msg->payload);
        fclose(log_file);
    }
    printf("MQTT received: %.*s\n", (int)msg->payloadlen, (char*)msg->payload);
}

void init_mqtt_client(struct mosquitto **mosq) {
    mosquitto_lib_init();
    *mosq = mosquitto_new(NULL, true, NULL);
    if (!*mosq) {
        fprintf(stderr, "Failed to create MQTT client\n");
        exit(1);
    }

    mosquitto_message_callback_set(*mosq, mqtt_message_callback);
}

void *mqtt_thread(void *arg) {
    struct mosquitto *mosq = NULL;
    init_mqtt_client(&mosq);

    if (mosquitto_connect(mosq, MQTT_BROKER, MQTT_PORT, 60) != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Failed to connect to MQTT broker\n");
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
        return NULL;
    }

    if (mosquitto_subscribe(mosq, NULL, MQTT_TOPIC, 0) != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Failed to subscribe to topic\n");
        mosquitto_disconnect(mosq);
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
        return NULL;
    }

    printf("MQTT client subscribed to %s\n", MQTT_TOPIC);

    while (mosquitto_loop(mosq, -1, 1) == MOSQ_ERR_SUCCESS) {
    }

    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    return NULL;
}