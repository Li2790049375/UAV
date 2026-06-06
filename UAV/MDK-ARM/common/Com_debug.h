#ifndef __COM_DEBUG_H__ 
#define __COM_DEBUG_H__

#include "usart.h"
#include <stdio.h>
#include "string.h"

// #define Debug_Log_ENABLE    1

#ifdef  Debug_Log_ENABLE

#define __FILE_NAME__   (strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '\\') + 1) : __FILE__)
#define __FILE_NAME     (strrchr(__FILE_NAME__, '/') ? (strrchr(__FILE_NAME__, '/') + 1) : __FILE_NAME__)

#define debug_printf(format, ...)   printf("[%s:%d] " format, __FILE_NAME, __LINE__, ##__VA_ARGS__)

#else
#define debug_printf(format, ...)

#endif 
#endif 
