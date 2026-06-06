#ifndef __TRANSMIT_H__
#define __TRANSMIT_H__

#include "NRF24L01.h"
#include "Data.h"
#include "FreeRTOS_Task.h"
#include "task.h"

#define Frame_Head_Check_1 'L'
#define Frame_Head_Check_2 'H'

void Transmit_Data(void);

#endif  
