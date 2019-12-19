#include "network.h"
#include "packet.h"

void on_passing(){
    static uint8_t highest_people_count = 0;

    switch(current_timing_protocol){
        case SEND_EVERY_PASSAGE:
            // todo: call the send function with the following parameter:
           //encode_people_count_packet()
            break;
        case SEND_CONSTANT_FREQUENCY:
            // Do nothing because sending happens with a constant frequency.
            break;
        case SEND_PEAK_AT_ZERO:
            if(get_people_count() > highest_people_count){
                highest_people_count++;
            } else if(get_people_count == 0){
                highest_people_count = 0;
                // TODO: implement the funtion that sends the followning paramter:
                // encode_people_count_packet()
            }
            break;
        case SEND_AFTER_INACTIVE_PEDIOD:
            // reset inactive timer.
            // TODO: implement timer
            break;
        default:
            // Things went wrong here.
    }
}

