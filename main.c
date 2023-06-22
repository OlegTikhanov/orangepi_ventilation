#include "settings.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "mqtt.h"
#include "ventilation.h"
#include "mhz19b.h"

void message_print(char *topic, char *message)
{
#if DEBUG > 1
	printf("%s: %s\n", topic, message);
#endif
	if (strcmp(topic, FAN_COMMAND_TOPIC) == 0)
	{
		switch (*message)
		{
		case '0':
			ventilation_set_fan(0);
			mqtt_send("0", FAN_STATE_TOPYC);
			mqtt_send("0", FAN_SPEED_STATE_TOPYC);
			mqtt_send("0", HEATER_STATE_TOPYC);
#if DEBUG > 0
			printf("%s\n", "Ventilation OFF");
#endif
			return;
		case '1':
			ventilation_set_fan(1);
			mqtt_send("1", FAN_STATE_TOPYC);
			mqtt_send("1", FAN_SPEED_STATE_TOPYC);
#if DEBUG > 0
			printf("%s\n", "Ventilation ON");
#endif
			return;
		}
	}

	if (strcmp(topic, HEATER_COMMAND_TOPIC) == 0)
	{
		switch (*message)
		{
		case '0':
			ventilation_set_heater(OFF);
			mqtt_send("0", HEATER_STATE_TOPYC);
#if DEBUG > 0
			printf("%s\n", "Heater OFF");
#endif
			return;
		case '1':
			ventilation_set_heater(ON);
			mqtt_send("1", HEATER_STATE_TOPYC);
#if DEBUG > 0
			printf("%s\n", "Heater ON");
#endif
			return;
		}
	}

	if (strcmp(topic, FAN_SPEED_COMMAND_TOPIC) == 0)
	{
		switch (*message)
		{
		case '0':
			ventilation_set_fan(0);
			mqtt_send("0", FAN_SPEED_STATE_TOPYC);
			mqtt_send("0", FAN_STATE_TOPYC);
			mqtt_send("0", HEATER_STATE_TOPYC);
#if DEBUG > 0
			printf("%s\n", "Выключение");
#endif
			return;
		case '1':
			ventilation_set_fan(1);
			mqtt_send("1", FAN_SPEED_STATE_TOPYC);
#if DEBUG > 0
			printf("%s\n", "Первая скорость");
#endif
			return;
		case '2':
			ventilation_set_fan(2);
			mqtt_send("2", FAN_SPEED_STATE_TOPYC);
#if DEBUG > 0
			printf("%s\n", "Вторая скорость");
#endif
			return;
		}
	}
}

uint8_t run = 1;
void shutdown(int sig)
{
#if DEBUG > 0
	printf("Остановка...\n");
#endif
	ventilation_set_fan(0);
	while (ventilation_get_status() != STOPED)
	{
	};
	run = 0;
}

void co_handler(void)
{
	co2_struct *mhz19;
	mhz19 = get_co();
	char co_buf[5];
	sprintf(co_buf, "%u", mhz19->co2);
	mqtt_send(co_buf, CO2_STATE_TOPYC);
	sprintf(co_buf, "%d", mhz19->temp);
	mqtt_send(co_buf, CO2_TEMP_STATE_TOPYC);
	sleep(10);
}

void connect_messages(void)
{
	sleep(10); // ждём загрузки homeassistant
	mqtt_send(FAN_CONFIG, FAN_CONFIG_TOPIC);
	mqtt_send(HEATER_CONFIG, HEATER_CONFIG_TOPIC);
	mqtt_send(CO2_CONFIG, CO2_CONFIG_TOPIC);
	mqtt_send(CO2_TEMP_CONFIG, CO2_TEMP_CONFIG_TOPIC);
	vent_state_typedef status = ventilation_get_status();
	if ((status == STARTING) ||
		(status == RUNNING) ||
		(status == VALVE_OPENING))
	{
		mqtt_send("1", FAN_STATE_TOPYC);
		mqtt_send("1", FAN_SPEED_STATE_TOPYC);
		mqtt_send("1", HEATER_STATE_TOPYC);
#if DEBUG > 0
		printf("ON on_connect\n");
#endif
	}
	else
	{
		mqtt_send("0", FAN_STATE_TOPYC);
		mqtt_send("0", HEATER_STATE_TOPYC);
		mqtt_send("0", FAN_SPEED_STATE_TOPYC);
#if DEBUG > 0
		printf("OFF on connect\n");
#endif
	}
}

int main(void)
{
	signal(SIGQUIT, shutdown);
	signal(SIGHUP, shutdown);
#if DEBUG > 0
	printf("App starting.\n");
#endif
	ventilation_init();
	mqtt_connect_function_set(connect_messages);
	mqtt_init();
	mqtt_message_callback_set(message_print);
	co_init();
#if DEBUG > 0
	printf("App started.\n");
#endif
	while (run)
	{
		co_handler();
	}
	return 0;
}
