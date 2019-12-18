#ifndef __PACKET_H
#define __PACKET_H

#include <stdint.h>

typedef enum{
    BATTERY_STATUS = 0b0,
    PEOPLE_COUNT = 0b1,
} packet_type_t;

typedef enum{
    BATTERY_GOOD = 0b00;
    BATTERY_LOW = 0b01;
    BATTERY_CRITICAL = 0b10;
} battery_status_t

typedef struct{
    packet_type_t type : 1;
    union{
        // To be used when BATTERY_STATUS is send.
        battery_status_t battery_status;

        // To be send when the PEOPLE_COUNT is send.
        uint8_t number_of_people;
    };

} LoRa_packet_t;


#endif /* __PACKET_H */