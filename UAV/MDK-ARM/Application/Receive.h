#ifndef __Receive_H__
#define __Receive_H__

#include "FreeRTOS.h"
#include "task.h"
#include "NRF24L01.h"
#include "Com_debug.h"
#include "Config.h"
#include "Flight.h"
#include "My_LC08.h"

#define Frame_Head_Check_1 'L'
#define Frame_Head_Check_2 'H'

uint8_t Receive_Data(Remote_Data *out_data);
void connect_state(uint8_t res);
void Flight_State_Update(void);

#endif
