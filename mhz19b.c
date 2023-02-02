#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include "mhz19b.h"

static char get_co_command[] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
static char response_buf[9];
static int fd;
int co_init(void)
{

    if ((fd = serialOpen(CO_DEVICE, 9600)) < 0)
    {
        fprintf(stderr, "Ошибка открытия порта: %s\n", strerror(errno));
        return 1;
    }

    if (wiringPiSetup() == -1)
    {
        fprintf(stdout, "Unable to start wiringPi: %s\n", strerror(errno));
        return 1;
    }
}

static char get_checksumm(char *packet)
{
    char i, checksum;
    for (i = 1; i < 8; i++)
    {
        checksum += packet[i];
    }
    checksum = 0xff - checksum;
    checksum += 1;
    return checksum;
}

//старт после сброса питания ~ 0сек
uint16_t get_co(void)
{
    fflush(stdout);
    for (uint8_t i = 0; i < sizeof(get_co_command) / sizeof(char); i++)
        serialPutchar(fd, get_co_command[i]);
    delay(RESPONSE_DALAY);
    uint8_t i = 0;
    while (serialDataAvail(fd))
    {
        response_buf[i] = serialGetchar(fd);
        fflush(stdout);
        i++;
    }
    if (response_buf[8] == get_checksumm(response_buf))
        return response_buf[2] * 256 + response_buf[3];
    return 0;
}
