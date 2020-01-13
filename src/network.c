#include "network.h"
#include "packet.h"
#include "board_definitions.h"
#include "hardware_functions.h"
#include "LoRa.h"
#include "vl53_main.h"

/*
T = 1/F
F = SysCoreClock / ((PSC-1) * (Period-1))
SysCoreClock = 2097152.

1/T = 2097152 / ((8001-1) * (Period-1))
2097152 * T = 1 * ((8000) * (Period-1)) 
Period = ((2091752 / prescaler) * T) + 1
*/
#define INACTIVE_TIMER_PRESCALER 8001
#define INACTIVE_TIMER_PERIOD (round(((2097152/INACTIVE_TIMER_PRESCALER) * PASSING_INACTIVE_TIME) + 1))

// Practicly the same calculation as with
// The inactive timer, but the number of seconds
// per etmal is devided as well.
#define CONTINUOUS_TIMER_PRESCALER 36001
#define CONTINUOUS_TIMER_PERIOD (round(((2097152/CONTINUOUS_TIMER_PRESCALER) * ((60*60*24)/CONSTANT_LORA_FREQUENCY)) + 1))

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

    switch(current_timing_protocol)
    {
        case SEND_EVERY_PASSAGE:
            network_send_people_count();
            break;
        case SEND_CONSTANT_FREQUENCY:
            // Do nothing because sending happens with a constant frequency.
            break;
        case SEND_PEAK_AT_ZERO:
            if(VL53_get_people_count() > highest_people_count)
            {
                highest_people_count = VL53_get_people_count();
            } else if(VL53_get_people_count() == 0){
                highest_people_count = 0;
                network_send_people_count();
            }
            break;
        case SEND_AFTER_INACTIVE_PEDIOD:
            // Start the timer_inactive at 0 so it will be exactly 30s.
            #ifdef LORA_BOARD
            TIM7->CNT = 0;
            #else
            // This function is not implemented for this board.
            #endif /* LORA_BOARD */
            enable_inactive_timer();
            break;
        default:
            // Things went wrong here.
            break;
    }
}

void network_send_people_count()
{
    LoRa_packet_t packet = encode_people_count_packet();
    #ifdef DEBUG
    UART_put_LoRaPacket(decode_frame(encode_frame(packet)));
    #endif /* DEBUG */
    LoRa_Send_Word(encode_frame(packet));
}

void initialise_inactive_timer()
{
    #ifdef LORA_BOARD
/*
    T = 30s so F = 0.033Hz
    TIMupdateFreq(HZ) = Clock/((PSC-1)*(Period-1))
    0.0333 = 2097152/((PSC-1)*(Period-1))
    62,914,623 = (PSC-1)*(Period-1)
    62,914,623 = (65,535) * (961)
    PSC = 8001
    Period = 7865
    */

    timer_inactive.Instance = TIM7;
    timer_inactive.Init.Prescaler = INACTIVE_TIMER_PRESCALER;
    timer_inactive.Init.Period = INACTIVE_TIMER_PERIOD;
    timer_inactive.Init.CounterMode = TIM_COUNTERMODE_UP;
    timer_inactive.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    #else
    // This is not implemented for this board
    #endif /* LORA_BOARD */
}


void initialise_continious_timer()
{
    #ifdef LORA_BOARD
/*
-- This calculation is not true anymore, since the period is variable.
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
    timer_continuous.Init.Prescaler = CONTINUOUS_TIMER_PRESCALER;
    timer_continuous.Init.Period = CONTINUOUS_TIMER_PERIOD;
    timer_continuous.Init.CounterMode = TIM_COUNTERMODE_UP;
    timer_continuous.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    __TIM6_CLK_ENABLE();
    HAL_TIM_Base_Init(&timer_continuous);
    __HAL_TIM_CLEAR_FLAG(&timer_continuous, TIM_IT_UPDATE);
    HAL_TIM_Base_Start_IT(&timer_continuous);

    HAL_NVIC_SetPriority(TIM6_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM6_IRQn);
    #else
    // This is not implemented for this board
    #endif /* LORA_BOARD */
}

void enable_inactive_timer()
{
    #ifdef LORA_BOARD
    #ifdef DEBUG
    UART_PutStr("Enabling inactive timer\n\r");
    #endif /* DEBUG */
    __TIM7_CLK_ENABLE();
    HAL_TIM_Base_Init(&timer_inactive);
    __HAL_TIM_CLEAR_FLAG(&timer_inactive, TIM_IT_UPDATE);
    HAL_TIM_Base_Start_IT(&timer_inactive);

    HAL_NVIC_SetPriority(TIM7_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM7_IRQn);
    #else
    // This function is not implemented yet for the disco board.
    #endif /* LORA_BOARD */
}

void disable_inactive_timer()
{
    #ifdef LORA_BOARD
    __TIM7_CLK_DISABLE();
    HAL_TIM_Base_DeInit(&timer_inactive);

    HAL_NVIC_DisableIRQ(TIM7_IRQn);
    #else
    // This is not implemented for the Disco board yet.
    #endif /* LORA_BOARD */
}

void disable_continuous_timer()
{
    #ifdef LORA_BOARD
    __TIM6_CLK_DISABLE();
    HAL_TIM_Base_DeInit(&timer_continuous);

    HAL_NVIC_DisableIRQ(TIM6_IRQn);
    #else
    // This functionality is not implemented yet.
    #endif /* LORA_BOARD */
}

void change_network_timing_protocol(network_timing_protocol_t change_to)
{
    // Disable inactive mode by default.
    disable_inactive_timer();

   // Disable continious timer.
    disable_continuous_timer();

    switch(change_to)
    {
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
