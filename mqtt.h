#ifndef MQTT_H
#define MQTT_H
#include "settings.h"





void mqtt_init(void);
int mqtt_send(char *msg, char *topic);
void mqtt_message_callback_set(void (*message_reader)(char * , char *));
void mqtt_connect_function_set(void (*function)(void));
#endif