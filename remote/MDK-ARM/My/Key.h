#ifndef __KEY_H__
#define __KEY_H__

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

typedef enum {
    Key_NUll = 0,
    Key1,
    Key2,
    Key3,
    Key4,
    Key5,
    Key6,
    Key7,
    Key8,
}Key_NUM;

Key_NUM KeyNum(void);

#endif
