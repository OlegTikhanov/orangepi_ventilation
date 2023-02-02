#include <stdio.h>
#include <mosquitto.h>
#include <stdlib.h>
#include <string.h>
#include "mqtt.h"
#include <unistd.h>

void (*message_func)(char *, char *) = NULL;
void (*on_connect_function)(void) = NULL;

void mqtt_message_callback_set(void (*message_reader)(char *, char *))
{
    message_func = message_reader;
}

void mqtt_connect_function_set(void (*function)(void))
{
    on_connect_function = function;
}

void mqtt_subscribe_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
    if (message_func != NULL)
    {
        message_func(message->topic, message->payload);
    }
}

void mosq_log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str)
{

    switch (level)
    {
    case MOSQ_LOG_DEBUG:
    case MOSQ_LOG_INFO:
    case MOSQ_LOG_NOTICE:
    case MOSQ_LOG_WARNING:
        break;
    case MOSQ_LOG_ERR:
    {
        printf("%i:%s\n", level, str);
    }
    }
}

void connect_callback(struct mosquitto *mosq, void *obj, int rc)
{
#if DEBUG > 0
    printf("Connect sucsess\n");
#endif
    mosquitto_subscribe(mosq, NULL, FAN_COMMAND_TOPIC, 0);
    mosquitto_subscribe(mosq, NULL, HEATER_COMMAND_TOPIC, 0);
    mosquitto_subscribe(mosq, NULL, FAN_SPEED_COMMAND_TOPIC, 0);
    if (on_connect_function != NULL)
    {
        on_connect_function();
    }
}

struct mosquitto *mosq = NULL;
void mqtt_init(void)
{
    int port = 1883;
    int keepalive = 60;
    bool clean_session = true;

    mosquitto_lib_init();
    mosq = mosquitto_new(NULL, clean_session, NULL);
    if (!mosq)
    {
        fprintf(stderr, "Error: Out of memory.\n");
        exit(1);
    }
    mosquitto_username_pw_set(mosq, MQTT_USER, MQTT_PASSWORD);

    mosquitto_log_callback_set(mosq, mosq_log_callback);

    if (mosquitto_connect(mosq, MQTT_HOST, port, keepalive))
    {
        fprintf(stderr, "Unable to connect.\n");
        exit(1);
    }
    int loop = mosquitto_loop_start(mosq);
    if (loop != MOSQ_ERR_SUCCESS)
    {
        fprintf(stderr, "Unable to start loop: %i\n", loop);
        exit(1);
    }
    mosquitto_message_callback_set(mosq, mqtt_subscribe_message);
    mosquitto_connect_callback_set(mosq, connect_callback);
}

int mqtt_send(char *msg, char *topic)
{
    return mosquitto_publish(mosq, NULL, topic, strlen(msg), msg, 1, 1);
}