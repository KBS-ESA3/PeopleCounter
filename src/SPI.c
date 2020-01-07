#include "SPI.h"
#include "sx1276Regs-LoRa.h"
#include "mlm32l0xx_hw_conf.h"

SPI_HandleTypeDef hspi;

void HW_SPI_Init(void)
{
  /*##-1- Configure the SPI peripheral */
  /* Set the SPI parameters */
  hspi.Instance = SPI1;

  hspi.Init.BaudRatePrescaler = SpiFrequency(10000000);
  hspi.Init.Direction = SPI_DIRECTION_2LINES;
  hspi.Init.Mode = SPI_MODE_MASTER;
  hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi.Init.NSS = SPI_NSS_SOFT;
  hspi.Init.TIMode = SPI_TIMODE_DISABLE;

  SPI_CLK_ENABLE();

  if (HAL_SPI_Init(&hspi) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /*##-2- Configure the SPI GPIOs */
  HW_SPI_IoInit();
}

void HW_SPI_IoInit(void)
{
  GPIO_InitTypeDef initStruct = {0};

  initStruct.Mode = GPIO_MODE_AF_PP;
  initStruct.Pull = GPIO_NOPULL;
  initStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  initStruct.Alternate = SPI1_AF;

  HW_GPIO_Init(RADIO_SCLK_PORT, RADIO_SCLK_PIN, &initStruct);
  HW_GPIO_Init(RADIO_MISO_PORT, RADIO_MISO_PIN, &initStruct);
  HW_GPIO_Init(RADIO_MOSI_PORT, RADIO_MOSI_PIN, &initStruct);

  initStruct.Mode = GPIO_MODE_OUTPUT_PP;
  initStruct.Pull = GPIO_NOPULL;

  HW_GPIO_Init(RADIO_NSS_PORT, RADIO_NSS_PIN, &initStruct);

  HAL_GPIO_WritePin(RADIO_NSS_PORT, RADIO_NSS_PIN, 1);
}

uint16_t HW_SPI_InOut(uint16_t txData)
{
  uint16_t rxData;

  HAL_SPI_TransmitReceive(&hspi, (uint8_t *)&txData, (uint8_t *)&rxData, 1, HAL_MAX_DELAY);

  return rxData;
}

static uint32_t SpiFrequency(uint32_t hz)
{
  uint32_t divisor = 0;
  uint32_t system_clock_temp = SystemCoreClock;
  uint32_t baudRate;

  while (system_clock_temp > hz)
  {
    divisor++;
    system_clock_temp = (system_clock_temp >> 1);

    if (divisor >= 7)
    {
      break;
    }
  }

  baudRate = (((divisor & 0x4) == 0) ? 0x0 : SPI_CR1_BR_2) |
             (((divisor & 0x2) == 0) ? 0x0 : SPI_CR1_BR_1) |
             (((divisor & 0x1) == 0) ? 0x0 : SPI_CR1_BR_0);

  return baudRate;
}

void HW_GPIO_Init(GPIO_TypeDef *port, uint16_t GPIO_Pin, GPIO_InitTypeDef *initStruct)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  initStruct->Pin = GPIO_Pin;

  HAL_GPIO_Init(port, initStruct);
}

void Error_Handler(void)
{
  while(1)
  {
      HAL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
      HAL_Delay(10);
  }
}
