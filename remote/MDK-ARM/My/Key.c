#include "Key.h"

Key_NUM KeyNum()
{
    if(HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(10);
        if(HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET)
        {
            while (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET)
            {
                vTaskDelay(1);
            }
            
            return Key1;
        }
    }
    else if(HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(10);
        if(HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_RESET)
        {
            while (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_RESET)
            {
                vTaskDelay(1);
            }
            return Key2;
        }
    }
    else if(HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(10);
        if(HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin) == GPIO_PIN_RESET)
        {
            while (HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin) == GPIO_PIN_RESET)
            {
                vTaskDelay(1);
            }
            return Key3;
        }
    }
    else if(HAL_GPIO_ReadPin(KEY4_GPIO_Port, KEY4_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(10);
        if(HAL_GPIO_ReadPin(KEY4_GPIO_Port, KEY4_Pin) == GPIO_PIN_RESET)
        {
            while (HAL_GPIO_ReadPin(KEY4_GPIO_Port, KEY4_Pin) == GPIO_PIN_RESET)
            {
                vTaskDelay(1);
            }
            return Key4;
        }
    }
    else if(HAL_GPIO_ReadPin(KEY5_GPIO_Port, KEY5_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(10);
        if(HAL_GPIO_ReadPin(KEY5_GPIO_Port, KEY5_Pin) == GPIO_PIN_RESET)
        {
            while (HAL_GPIO_ReadPin(KEY5_GPIO_Port, KEY5_Pin) == GPIO_PIN_RESET)
            {
                vTaskDelay(1);
            }
            return Key5;
        }
    }
    else if(HAL_GPIO_ReadPin(KEY6_GPIO_Port, KEY6_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(10);
        if(HAL_GPIO_ReadPin(KEY6_GPIO_Port, KEY6_Pin) == GPIO_PIN_RESET)
        {
            while (HAL_GPIO_ReadPin(KEY6_GPIO_Port, KEY6_Pin) == GPIO_PIN_RESET)
            {
                vTaskDelay(1);
            }
            return Key6;
        }
    }
    else if(HAL_GPIO_ReadPin(KEY7_GPIO_Port, KEY7_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(10);
        if(HAL_GPIO_ReadPin(KEY7_GPIO_Port, KEY7_Pin) == GPIO_PIN_RESET)
        {
            while (HAL_GPIO_ReadPin(KEY7_GPIO_Port, KEY7_Pin) == GPIO_PIN_RESET)
            {
                vTaskDelay(1);
            }
            return Key7;
        }
    }
    else if(HAL_GPIO_ReadPin(KEY8_GPIO_Port, KEY8_Pin) == GPIO_PIN_RESET)
    {
        vTaskDelay(10);
        if(HAL_GPIO_ReadPin(KEY8_GPIO_Port, KEY8_Pin) == GPIO_PIN_RESET)
        {
            while (HAL_GPIO_ReadPin(KEY8_GPIO_Port, KEY8_Pin) == GPIO_PIN_RESET)
            {
                vTaskDelay(1);
            }
            return Key8;
        }
    }

   return Key_NUll;
}
