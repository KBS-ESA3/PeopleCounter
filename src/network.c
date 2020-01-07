#include "network.h"
#include "packet.h"
#include "board_definitions.h"

network_timing_protocol_t current_timing_protocol = INITIAL_NETWORK_TIMING_PROTOCOL;

TIM_HandleTypeDef timer_inactive;
TIM_HandleTypeDef timer_continuous;

// function prototype.
void initialise_inactive_timer();
void initialise_continuous_timer();
void enable_inactive_timer();

void on_passing()
{
    static uint8_t highest_people_count = 0;

    switch(current_timing_protocol){
        case SEND_EVERY_PASSAGE:
            network_send_people_count();
            break;
        case SEND_CONSTANT_FREQUENCY:
            // Do nothing because sending happens with a constant frequency.
            break;
        case SEND_PEAK_AT_ZERO:
            if(get_people_count() > highest_people_count){
                highest_people_count++;
            } else if(get_people_count() == 0){
                highest_people_count = 0;
                network_send_people_count();
            }
            break;
        case SEND_AFTER_INACTIVE_PEDIOD:
            // Start the timer_inactive at 0 so it will be exactly 30s.
            TIM7->CNT = 0;
            enable_inactive_timer();
            break;
        default:
            // Things went wrong here.
            break;
    }
}

void network_send_people_count()
{
    // TODO: implement the funtion that sends the followning paramter:
    // encode_people_count_packet()
}


void initialise_inactive_timer()
{
/*
    T = 30s so F = 0.033Hz
    TIMupdateFreq(HZ) = Clock/((PSC-1)*(Period-1))
    0.0333 = 2097152/((PSC-1)*(Period-1))
    62,914,623 = (PSC-1)*(Period-1)
    62,914,623 = (65,535) * (961)
    PSC = 8001
    Period = 7865
    */

   // TODO - Let this thing run just once, not continious.
    timer_inactive.Instance = TIM7;
    timer_inactive.Init.Prescaler = 8000;
    timer_inactive.Init.Period = 7864;
    timer_inactive.Init.CounterMode = TIM_COUNTERMODE_UP;
    timer_inactive.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
}


void initialise_continious_timer()
{
/*
    140 timer per 86400 seconds
    F = 0.0016Hz
    TIMupdateFreq(HZ) = Clock/((PSC-1)*(Period-1))
    0.0016 = 2097152/((PSC-1)*(Period-1))
    1,294,242,377 = (PSC-1)*(Period-1)
    1,294,242,377 = (36,001) * (35,952)
    PSC = 36001
    Period = 35952
*/

    timer_continuous.Instance = TIM6;
    timer_continuous.Init.Prescaler = 36001;
    timer_continuous.Init.Period = 35952;
    timer_continuous.Init.CounterMode = TIM_COUNTERMODE_UP;
    timer_continuous.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    __TIM6_CLK_ENABLE();
    HAL_TIM_Base_Init(&timer_continuous);
    HAL_TIM_Base_Start_IT(&timer_continuous);

    HAL_NVIC_SetPriority(TIM6_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM6_IRQn);
}

void enable_inactive_timer()
{
    __TIM7_CLK_ENABLE();
    HAL_TIM_Base_Init(&timer_inactive);
    HAL_TIM_Base_Start_IT(&timer_inactive);

    HAL_NVIC_SetPriority(TIM7_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM7_IRQn);
}

void disable_inactive_timer()
{
    __TIM7_CLK_DISABLE();
    HAL_TIM_Base_DeInit(&timer_inactive);

    HAL_NVIC_DisableIRQ(TIM7_IRQn);
}

void disable_continuous_timer()
{
    __TIM6_CLK_DISABLE();
    HAL_TIM_Base_DeInit(&timer_continuous);

    HAL_NVIC_DisableIRQ(TIM6_IRQn);
}

void change_network_timing_protocol(network_timing_protocol_t change_to)
{
    // Disable inactive mode by default.
   disable_inactive_timer();

   // Disable continious timer.
    disable_continuous_timer();

    switch(change_to){
        case SEND_EVERY_PASSAGE:
            break;
        case SEND_CONSTANT_FREQUENCY:
            // Initialise a timer with the frecuency defined in CONSTANT_LORA_FREQUENCY.
            initialise_continious_timer();
            break;
        case SEND_PEAK_AT_ZERO:
            break;
        case SEND_AFTER_INACTIVE_PEDIOD:
            initialise_inactive_timer();
            break;
        default:
            // Things went wrong here.
            break;
    }

    current_timing_protocol = change_to;
}

