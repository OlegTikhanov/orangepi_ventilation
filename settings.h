#ifndef SETTINGS_H
#define SETTINGS_H

#define DEBUG 1 //0,1,2
// Для вентиляции
#define PA20 25
#define PA10 23
#define PA9 22
#define PA8 20
#define PA7 19

#define VALVE_DELAY 15
#define HEATER_DELAY 30



#define FAN PA10
#define FAN_LOW_SPEED PA20
#define VALVE PA8
#define VALVE_POWER PA9
#define HEATER PA7

#define CO_DEVICE "/dev/ttyS1"

// Для MQTT
#define MQTT_HOST "127.0.0.1"
#define MQTT_USER "user"
#define MQTT_PASSWORD "password"



#define FAN_CONFIG_TOPIC "homeassistant/fan/room/ventilation/config"
#define HEATER_CONFIG_TOPIC "homeassistant/switch/room/ventilation_heater/config"
#define CO2_CONFIG_TOPIC "homeassistant/sensor/room/ventilation_co2/config"

// топики
#define ROOT_TOPIC "room/ventilation/"
#define FAN_COMMAND_TOPIC ROOT_TOPIC "fan/set"
#define FAN_STATE_TOPYC ROOT_TOPIC "fan/state"
#define FAN_SPEED_COMMAND_TOPIC ROOT_TOPIC "fan/speed"
#define FAN_SPEED_STATE_TOPYC ROOT_TOPIC "fan/speed_state"

#define HEATER_COMMAND_TOPIC ROOT_TOPIC "heater/set"
#define HEATER_STATE_TOPYC ROOT_TOPIC "heater/state"

#define CO2_STATE_TOPYC ROOT_TOPIC "co2/state"

//Конфиги

#define DEVICE_CONFIG \
"{\
	\"identifiers\":\"ventilation\",\
	\"name\":\"Приточная вентиляция\",\
	\"model\":\"pvvco2\"\
}"

#define FAN_CONFIG \
"{\
	\"name\":\"Приточная вентиляция\",\
	\"device_class\":\"fan\",\
	\"unique_id\":\"ventilation_fan\",\
	\"object_id\":\"ventilation_fan\",\
	\"command_topic\":\"" FAN_COMMAND_TOPIC "\",\
	\"state_topic\":\"" FAN_STATE_TOPYC "\",\
	\"percentage_command_topic\":\"" FAN_SPEED_COMMAND_TOPIC "\",\
    \"percentage_state_topic\":\"" FAN_SPEED_STATE_TOPYC "\",\
	\"payload_on\":\"1\",\
    \"payload_off\":\"0\",\
    \"speed_range_min\":\"1\",\
    \"speed_range_max\":\"2\",\
	\"device\":" DEVICE_CONFIG "\
}"

#define HEATER_CONFIG \
"{\
	\"name\":\"Нагреватель\",\
	\"unique_id\":\"ventilation_heater\",\
	\"object_id\":\"ventilation_heater\",\
	\"command_topic\":\"" HEATER_COMMAND_TOPIC "\",\
    \"state_topic\":\"" HEATER_STATE_TOPYC "\",\
	\"payload_on\":\"1\",\
    \"payload_off\":\"0\",\
	\"device\":" DEVICE_CONFIG "\
}"

#define CO2_CONFIG \
"{\
	\"name\":\"CO2\",\
	\"unit_of_measurement\":\"ppm\",\
	\"device_class\":\"carbon_dioxide\",\
	\"unique_id\":\"ventilation_co2\",\
	\"object_id\":\"co2\",\
	\"state_topic\":\"" CO2_STATE_TOPYC "\",\
	\"device\":" DEVICE_CONFIG "\
}"

#endif
