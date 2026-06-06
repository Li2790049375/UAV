#ifndef __MY_SPI_H__
#define __MY_SPI_H__

#include "main.h"

#define     SPI_SCK_H       SPI_SCK_GPIO_Port->BSRR  = SPI_SCK_Pin
#define     SPI_SCK_L       SPI_SCK_GPIO_Port->BRR   = SPI_SCK_Pin
#define     SPI_MOSI_H      SPI_MOSI_GPIO_Port->BSRR = SPI_MOSI_Pin
#define     SPI_MOSI_L      SPI_MOSI_GPIO_Port->BRR  = SPI_MOSI_Pin
#define     MISO            ((GPIOB->IDR & SPI_MISO_Pin) ? 1:0)

uint8_t SPI_RW(uint8_t dat);

#endif
