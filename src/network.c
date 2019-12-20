#include "network.h"
#include "packet.h"
#include "board_definitions.h"

network_timing_protocol_t current_timing_protocol = INITIAL_NETWORK_TIMING_PROTOCOL;

// function prototype.
void network_send();

void on_passing(){
    static uint8_t highest_people_count = 0;

    switch(current_timing_protocol){
        case SEND_EVERY_PASSAGE:
            network_send();
            break;
        case SEND_CONSTANT_FREQUENCY:
            // Do nothing because sending happens with a constant frequency.
            break;
        case SEND_PEAK_AT_ZERO:
            if(get_people_count() > highest_people_count){
                highest_people_count++;
            } else if(get_people_count() == 0){
                highest_people_count = 0;
                network_send();
            }
            break;
        case SEND_AFTER_INACTIVE_PEDIOD:
            // reset inactive timer.
            // TODO: implement timer
            break;
        default:
            // Things went wrong here.
            break;
    }
}

void network_send(){
    // TODO: implement the funtion that sends the followning paramter:
    // encode_people_count_packet()
}

void FooBar(){
    // initialise timer 30 seconds

    __TIM7_CLK_ENABLE();
    TIM_HandleTypeDef timer;
    
    timer.Instance = TIM7;
    timer.Init.Prescaler = 10000;
    timer.Init.Period = 50000;
    timer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    timer.Init.CounterMode = TIM_COUNTERMODE_UP;
    HAL_TIM_Base_Init(&timer);
    HAL_TIM_Base_Start_IT(&timer);

    HAL_NVIC_SetPriority(TIM7_IRQn, 0, 0);
    HAL_NVIC_DisableIRQ(TIM7_IRQn);

}

