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



