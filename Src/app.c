#include "stm32f3xx_hal.h"

uint8_t InputBuffer1[4096] = {0};
uint8_t InputBuffer2[4096] = {0};
uint16_t numBytes1 = 0;
uint16_t numBytes2 = 0;
uint8_t DmaOk1 = 0;
uint8_t DmaOk2 = 0;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

void SetISR1()
{
  if (HAL_UART_Receive_DMA(&huart1, InputBuffer1, 4096)==HAL_OK)
  {
    SET_BIT(huart1.Instance->CR1, USART_CR1_IDLEIE);
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_RESET);
  }
}

void SetISR2()
{
  if (HAL_UART_Receive_DMA(&huart2, InputBuffer2, 4096)==HAL_OK)
  {
    SET_BIT(huart2.Instance->CR1, USART_CR1_IDLEIE);
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
  }
}

//------------------------------------------------------------------------------
// Callback functions Transmit UART-DMA
void UART2_DMATransmitCplt(DMA_HandleTypeDef *hdma)
{
  HAL_UART_DMAStop(&huart2);
}

void UART1_DMATransmitCplt(DMA_HandleTypeDef *hdma)
{
  HAL_UART_DMAStop(&huart1);
}

//------------------------------------------------------------------------------
// Init function
void AppInit()
{
  SetISR1();
  SetISR2();  
}

//------------------------------------------------------------------------------
// Work function
void AppWork()
{
  SetISR1();
  SetISR2();
  if(DmaOk1 == 1)
  {
    DmaOk1 = 0;
    if(numBytes1 > 0)
    {
    if(HAL_UART_Transmit_DMA(&huart2, InputBuffer1, numBytes1)==HAL_OK)
        huart2.hdmatx->XferCpltCallback = UART2_DMATransmitCplt;
      else
        HAL_UART_DMAStop(&huart2);
    }
  }
  if(DmaOk2 == 1)
  {
    DmaOk2 = 0;
    if(numBytes2 > 0)
    {
    if(HAL_UART_Transmit_DMA(&huart1, InputBuffer2, numBytes2)==HAL_OK)
        huart1.hdmatx->XferCpltCallback = UART2_DMATransmitCplt;
    else
       HAL_UART_DMAStop(&huart2);
    }
  }
}
