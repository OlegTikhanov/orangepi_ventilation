#ifndef MHZ19B_H
#define MHZ19B_H

#include "settings.h"
#define RESPONSE_DALAY 1000

typedef struct {
    uint16_t co2;
    uint8_t  temp;
} co2_struct;


int co_init(void);
co2_struct* get_co(void);

#endif