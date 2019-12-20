#include "packet.h"
#include "network.h"

uint8_t people_count = 0;

void increment_people_count(void){
    people_count++;
    on_passing();
}

void decrease_people_count(void){
    people_count--;
    on_passing();
}

uint8_t get_people_count(){
    return people_count;
}

LoRa_packet_t encode_people_count_packet(){
    LoRa_packet_t packet = {0,0};
    packet.type = TYPE_PEOPLE_COUNT;
    packet.number_of_people =  people_count;
    return packet;
}

LoRa_packet_t encode_battery_status_packet(){
    LoRa_packet_t packet = {0,0};
    packet.type = TYPE_BATTERY_STATUS;
    packet.number_of_people =  people_count;
    return packet;
}
