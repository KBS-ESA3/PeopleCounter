#include "packet.h"
#include "network.h"
#include "hardware_functions.h"

uint8_t people_count = 0;

void increment_people_count(void)
{
    people_count++;
    on_passing();
}

void decrease_people_count(void)
{
    people_count--;
    on_passing();
}

uint8_t get_people_count(){
    return people_count;
}

LoRa_packet_t encode_people_count_packet()
{
    LoRa_packet_t packet = {0,0};
    packet.type = TYPE_PEOPLE_COUNT;
    packet.number_of_people =  people_count;
    return packet;
}

LoRa_packet_t encode_battery_status_packet()
{
    LoRa_packet_t packet = {0,0};
    packet.type = TYPE_BATTERY_STATUS;
    packet.number_of_people =  people_count;
    return packet;
}

uint16_t encode_frame(LoRa_packet_t packet){
    uint16_t frame = 0;
    frame |= packet.type << 8;
    if(packet.type == TYPE_PEOPLE_COUNT)
    {
        frame |= packet.number_of_people;
    } else if(packet.type  == TYPE_BATTERY_STATUS)
    {
        frame |= packet.battery_status;
    }   else {
        UART_PutStr("There was a problem encoding this frame.");
    }

    return frame;
}

LoRa_packet_t decode_frame(uint16_t frame)
{
    LoRa_packet_t packet = {0,0};
    if(frame & (TYPE_PEOPLE_COUNT << 8))
    {
        packet.type = TYPE_PEOPLE_COUNT;
        packet.number_of_people = (uint8_t)frame;
    }  else {
        packet.type = TYPE_BATTERY_STATUS;
        packet.battery_status = (uint8_t)frame;
    }
    return packet;
}
