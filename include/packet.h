#ifndef __PACKET_H
#define __PACKET_H

#include <stdint.h>

// The different possible packet types.
typedef enum{
    TYPE_BATTERY_STATUS = 0b0,
    TYPE_PEOPLE_COUNT = 0b1,
} packet_type_t;

// The different possible variables for the battery status.
typedef enum{
    BATTERY_GOOD = 0b00,
    BATTERY_LOW = 0b01,
    BATTERY_CRITICAL = 0b10,
} battery_status_t;

// The content of a packet that needs to be send.
typedef struct{
    packet_type_t type : 1;
    union{
        // To be used when BATTERY_STATUS is send.
        battery_status_t battery_status : 2;

        // To be send when the PEOPLE_COUNT is send.
        uint8_t number_of_people;
    };

} LoRa_packet_t;

void increment_people_count(void);
void decrease_people_count(void);
uint8_t get_people_count();
LoRa_packet_t encode_people_count_packet();
LoRa_packet_t encode_battery_status_packet();
uint16_t encode_frame(LoRa_packet_t);
LoRa_packet_t decode_frame(uint16_t);

#endif /* __PACKET_H */