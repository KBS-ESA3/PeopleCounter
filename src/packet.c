#include "packet.h"
#include "network.h"
#include "hardware_functions.h"
#include "defines.h"
#include "vl53_main.h"
#include "battery.h"

LoRa_packet_t encode_people_count_packet()
{
    LoRa_packet_t packet = {0,0,0};
    packet.device_id = DEVICE_ID;
    packet.type = TYPE_PEOPLE_COUNT;
    packet.number_of_people =  VL53_get_people_count();
    return packet;
}

LoRa_packet_t encode_battery_status_packet()
{
    LoRa_packet_t packet = {0,0,0};
    packet.device_id = DEVICE_ID;
    packet.type = TYPE_BATTERY_STATUS;
    packet.battery_status = battery_get_status();
    
    return packet;
}

uint16_t encode_frame(LoRa_packet_t packet){
    uint16_t frame = 0;
    frame |= packet.device_id << 9;
    frame |= packet.type << 8;
    if(packet.type == TYPE_PEOPLE_COUNT)
    {
        frame |= packet.number_of_people;
    } 
    else if(packet.type  == TYPE_BATTERY_STATUS)
    {
        frame |= packet.battery_status;
    }   
    else {
        #ifdef DEBUG
        UART_PutStr("There was a problem encoding this frame.");
        #endif /* DEBUG */
    }
    return frame;
}

LoRa_packet_t decode_frame(uint16_t frame)
{
    LoRa_packet_t packet = {0,0,0};
    if(frame & (TYPE_PEOPLE_COUNT << 8))
    {
        packet.type = TYPE_PEOPLE_COUNT;
        packet.number_of_people = (uint8_t)frame;
    }  
    else {
        packet.type = TYPE_BATTERY_STATUS;
        packet.battery_status = (uint8_t)frame;
    }

    packet.device_id = ((uint8_t)(frame >> 9));
    return packet;
}
