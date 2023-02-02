#include "ventilation.h"
#include <wiringPi.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>

#define fan(state) digitalWrite(FAN, state)
#define fan_state() ~digitalRead(FAN) & 1

#define fan_low_speed(state) digitalWrite(FAN_LOW_SPEED, state)
#define fan_low_speed_state() ~digitalRead(FAN_LOW_SPEED) & 1

#define valve(state) digitalWrite(VALVE, state)
#define valve_state() ~digitalRead(VALVE) & 1

#define valve_power(state) digitalWrite(VALVE_POWER, state)
#define valve_power_state() ~digitalRead(VALVE_POWER) & 1

#define heater(state) digitalWrite(HEATER, state)
#define heater_state() ~digitalRead(HEATER) & 1

#if DEBUG > 1
char debug_state_array[][15] = {"STARTING",
                                "RUNNING",
                                "VALVE_OPENING",
                                "VALVE_CLOSING",
                                "HEATER_STOP",
                                "HEATER_COOLING",
                                "STOPING",
                                "STOPED"};
#endif
static uint8_t working = 1;

static void fan_speed(uint8_t speed);

static vent_state_typedef vent_state = STOPED;

static vent_status_typedef status = {0, OFF};
static vent_status_typedef commands = {0, OFF};

static uint32_t timer;
static pthread_mutex_t timer_mutex;

vent_state_typedef ventilation_get_status(void)
{
    return vent_state;
}

void ventilation_set_fan(uint8_t speed)
{
    if ((speed >= 0) && (speed <= 2))
    {
        commands.fan = speed;
    }
}

void ventilation_set_heater(uint8_t state)
{
    if ((state == ON) || (state == OFF))
    {
        commands.heater = state;
    }
}

static void fan_speed(uint8_t speed)
{
    switch (speed)
    {
    case 0:
        if (status.heater == ON)
            return;
        status.fan = 0;
        fan_low_speed(OFF);
        fan(OFF);
        return;
    case 1:
        status.fan = 1;
        fan(ON);
        fan_low_speed(ON);
        return;
    case 2:
        status.fan = 2;
        fan(ON);
        fan_low_speed(OFF);
        return;
    }
}

//--------------------------------------------------

static void *timer_handler(void *data)
{
    while (working)
    {
        sleep(1);
        pthread_mutex_lock(&timer_mutex);
        if (timer > 0)
            timer--;
        pthread_mutex_unlock(&timer_mutex);
    }
}

static void set_timeout(uint32_t timeout)
{
    pthread_mutex_lock(&timer_mutex);
    timer = timeout;
    pthread_mutex_unlock(&timer_mutex);
}

static uint8_t timeout(void)
{
    pthread_mutex_lock(&timer_mutex);
    uint32_t tmp_timeout = timer;
    pthread_mutex_unlock(&timer_mutex);
    if (tmp_timeout == 0)
        return 1;
    return 0;
}

//--------------------------------------------------

static void *ventilation_handler(void *data)
{

    while (working)
    {
#if DEBUG > 1
        static vent_state_typedef old_state = 999;
        if (vent_state != old_state)
        {
            printf("State: %s\n", debug_state_array[vent_state]);
            old_state = vent_state;
        }
#endif
        delay(HANDLER_PAUSE);
        switch (vent_state)
        {
        case STOPED:
            if (commands.fan > 0)
                vent_state = STARTING;
            break;

        case STARTING:
            valve(ON);
            valve_power(ON);
            set_timeout(VALVE_DELAY);
            vent_state = VALVE_OPENING;
            break;

        case VALVE_OPENING:
            if (timeout())
            {
                valve_power(OFF);
                valve(OFF);
                fan_speed(status.fan = commands.fan);
                heater(status.heater = commands.heater);
                vent_state = RUNNING;
                break;
            }
            if (commands.fan == 0)
                vent_state = VALVE_CLOSING;
            break;

        case RUNNING:
            if (commands.fan == 0)
            {
                vent_state = STOPING;
                break;
            }
            if (status.fan != commands.fan)
            {
                fan_speed(status.fan = commands.fan);
            }
            if (status.heater != commands.heater)
            {
                if (commands.heater == OFF)
                {
                    vent_state = HEATER_STOP;
                    break;
                }
                heater(status.heater = commands.heater);
            }
            break;

        case STOPING:
            if (status.heater == ON)
            {
                vent_state = HEATER_STOP;
                break;
            }
            fan_speed(status.fan = 0);
            valve(OFF);
            valve_power(ON);
            vent_state = VALVE_CLOSING;
            set_timeout(VALVE_DELAY);
            break;

        case HEATER_STOP:
            heater(OFF);
            set_timeout(HEATER_DELAY);
            vent_state = HEATER_COOLING;
            break;

        case HEATER_COOLING:
            if (timeout())
            {
                status.heater = OFF;
                vent_state = RUNNING;
                set_timeout(VALVE_DELAY);
                break;
            }
            if (status.fan != commands.fan)
            {
                fan_speed(status.fan = commands.fan);
            }
            if ((commands.heater == ON) && (commands.fan > 0))
            {
                vent_state = VALVE_OPENING;
                set_timeout(0);
            }
            break;

        case VALVE_CLOSING:
            if (timeout())
            {
                valve_power(OFF);
                valve(OFF);
                vent_state = STOPED;
                break;
            }
            if (commands.fan > 0)
                vent_state = STARTING;
            break;
        }
    }
}

static pthread_t pthr_ventilation, pthr_timer;
void ventilation_init(void)
{
    // GPIO settings
    wiringPiSetup();

    pinMode(FAN, OUTPUT);
    fan(OFF);

    pinMode(FAN_LOW_SPEED, OUTPUT);
    fan_low_speed(OFF);

    pinMode(VALVE, OUTPUT);
    valve(OFF);

    pinMode(VALVE_POWER, OUTPUT);
    valve_power(OFF);

    pinMode(HEATER, OUTPUT);
    heater(OFF);

    //---------Запуск обработчика------------
    pthread_mutex_init(&timer_mutex, NULL);
    pthread_create(&pthr_ventilation, NULL, ventilation_handler, NULL);
    pthread_create(&pthr_timer, NULL, timer_handler, NULL);
    pthread_detach(pthr_ventilation);
    pthread_detach(pthr_timer);
}