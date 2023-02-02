#ifndef VENTILATION_H
#define VENTILATION_H

#include "settings.h"
#include <stdint.h>
#include <wiringPi.h>

#define ON LOW
#define OFF HIGH

#define HANDLER_PAUSE 100 // ms

typedef enum
{
    STARTING,
    RUNNING,
    VALVE_OPENING,
    VALVE_CLOSING,
    HEATER_STOP,
    HEATER_COOLING,
    STOPING,
    STOPED
} vent_state_typedef;


typedef struct
{
    uint8_t fan;
    uint8_t heater;
} vent_status_typedef;

void ventilation_set_fan(uint8_t speed);    // 0,1,2
void ventilation_set_heater(uint8_t state); // ON,OFF

vent_state_typedef ventilation_get_status(void);
void ventilation_set(vent_status_typedef command);
void ventilation_init(void);
#endif