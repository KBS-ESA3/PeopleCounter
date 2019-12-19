#ifndef __NETWORK_H
#define __NETWORK_H

// The number of times per day that the people_count needs to be send.
#define CONSTANT_LORA_FREQUENCY 140

// The number of seconds after which inactivity the number of people will be send.
#define PASSING_INACTIVE_TIME 30

#define INITIAL_NETWORK_TIMING_PROTOCOL SEND_EVERY_PASSAGE;

typedef enum{
    // With this mode the number of people in the room is
    // send everytime someone enters or exits the room.
    SEND_EVERY_PASSAGE,

    // In this mode the people_count is send a constant number of
    // times a day. 
    SEND_CONSTANT_FREQUENCY,

    // Send the highest number of people in the room since the last
    // time the room was completely empty.
    SEND_PEAK_AT_ZERO,

    //Send people_count after there was no passage for PASSING_INACTIVE_TIME.
    SEND_AFTER_INACTIVE_PEDIOD,

} network_timing_protocol_t;

network_timing_protocol_t current_timing_protocol = INITIAL_NETWORK_TIMING_PROTOCOL;

void on_passing();


#endif /* __NETWORK_H */