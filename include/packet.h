#ifndef __PACKET_H
#define __PACKET_H

#include <stdint.h>
#include "network.h"

// The different possible packet types.
typedef enum{
    TYPE_SWITCH_ALGORITHM_CONFIRMATION = 0b00,
    TYPE_BATTERY_STATUS = 0b01,
    TYPE_PEOPLE_COUNT = 0b10,
    TYPE_SWITCH_ALGORITHM_REQUEST = 0b11,
} packet_type_t;

// The different possible variables for the battery status.
typedef enum{
    BATTERY_GOOD = 0b00,
    BATTERY_LOW = 0b01,
    BATTERY_CRITICAL = 0b10,
} battery_status_t;

// The content of a packet that needs to be send.
typedef struct{
    uint8_t device_id : 6;
    packet_type_t type : 2;
    union{
        // To be used when BATTERY_STATUS is send.
        battery_status_t battery_status : 2;

        // To be send when the PEOPLE_COUNT is send.
        uint8_t number_of_people;

        // To be send when TYPE_SWITCH_ALGORITHM is send.
        network_timing_protocol_t network_protocol : 2;
    };

} LoRa_packet_t;


LoRa_packet_t encode_people_count_packet();
LoRa_packet_t encode_battery_status_packet();
LoRa_packet_t encode_switch_algorithm_confirmation_packet(network_timing_protocol_t protocol);
LoRa_packet_t encode_switch_algorithm_request_packet(network_timing_protocol_t protocol);
uint16_t encode_frame(LoRa_packet_t packet);
LoRa_packet_t decode_frame(uint16_t frame);

#endif /* __PACKET_H */
