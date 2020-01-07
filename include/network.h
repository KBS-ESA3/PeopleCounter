#ifndef __NETWORK_H
#define __NETWORK_H

// The number of times per day that the people_count needs to be send.
#define CONSTANT_LORA_FREQUENCY 140

// The number of seconds after which inactivity the number of people will be send.          -- THIS VARIABLETY IS NOT IMPLEMENTED YET
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

// This function is called when someone passes the door.
void on_passing();
void change_network_timing_protocol(network_timing_protocol_t change_to);
void disable_inactive_timer();
void network_send_people_count();


#endif /* __NETWORK_H */