#include "packet.h"

uint8_t PEOPLE_COUNT = 0;

void increment_people_count(void){
    PEOPLE_COUNT++;
}

void decrease_people_count(void){
    PEOPLE_COUNT--;
}

uint8_t get_people_count(){
    return PEOPLE_COUNT;
}

LoRa_packet_t encode_people_count_packet(){
    LoRa_packet_t packet = {0,0};
    packet.type = TYPE_PEOPLE_COUNT;
    packet.number_of_people =  PEOPLE_COUNT;
    return packet;
}

LoRa_packet_t encode_battery_status_packet(){
    LoRa_packet_t packet = {0,0};
    packet.type = TYPE_BATTERY_STATUS;
    packet.number_of_people =  PEOPLE_COUNT;
    return packet;
}


