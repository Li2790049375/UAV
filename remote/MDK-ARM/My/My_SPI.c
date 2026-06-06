#include "My_SPI.h"

uint8_t SPI_RW(uint8_t byte)
{
		uint8_t i;
		uint8_t Temp = 0x00;
		for (i = 0; i < 8; i++)
		{
				SPI_SCK_L;
				if (byte & 0x80)
				{
					SPI_MOSI_H; 
				}
				else
				{
					SPI_MOSI_L;
				}

				byte <<= 1;
				Temp <<= 1;
				
				SPI_SCK_H; 
				
				if(MISO) 
				{
					Temp++;
				}
				SPI_SCK_L;	
		}
		return Temp; 
}
