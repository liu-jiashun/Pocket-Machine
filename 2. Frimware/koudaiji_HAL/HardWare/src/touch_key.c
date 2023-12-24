#include "touch_key.h"

GPIO_Struct Touch_Key_Up, Touch_Key_Down, Touch_Key_Left, Touch_Key_Right, Touch_Key_A, Touch_Key_B, Touch_Key_C, Touch_Key_D;  //触摸按键接口的结构体
TOUCH_KEY_Enum Touch_Key_Value = TOUCH_KEY_NONE;  //当前触摸按键的键值

/*触摸按键接口初始化，配置成上拉输入模式*/
void Touch_Key_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    Touch_Key_Up.GPIOx = GPIOG;
    Touch_Key_Up.GPIO_Pin = GPIO_PIN_11;
    Touch_Key_Down.GPIOx = GPIOD;
    Touch_Key_Down.GPIO_Pin = GPIO_PIN_6;
    Touch_Key_Left.GPIOx = GPIOG;
    Touch_Key_Left.GPIO_Pin = GPIO_PIN_10;
    Touch_Key_Right.GPIOx = GPIOD;
    Touch_Key_Right.GPIO_Pin = GPIO_PIN_3;
    Touch_Key_A.GPIOx = GPIOA;
    Touch_Key_A.GPIO_Pin = GPIO_PIN_0;
    Touch_Key_B.GPIOx = GPIOG;
    Touch_Key_B.GPIO_Pin = GPIO_PIN_6;
    Touch_Key_C.GPIOx = GPIOG;
    Touch_Key_C.GPIO_Pin = GPIO_PIN_7;
    Touch_Key_D.GPIOx = GPIOG;
    Touch_Key_D.GPIO_Pin = GPIO_PIN_8;

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/*触摸按键扫描*/
void Touch_Key_Scan(void)
{
    static union key_data //键值的共用体
    {
        struct key_bit
        {
            uint8_t data_up    :1;
            uint8_t data_down  :1;
            uint8_t data_left  :1;
            uint8_t data_right :1;
            uint8_t data_A     :1;
            uint8_t data_B     :1;
            uint8_t data_C     :1;
            uint8_t data_D     :1;  //每个按键值用位域的一个位表示
        }key_bit_struct;
        uint8_t key_code;
    }key_data_struct;

    static uint8_t num1 = 0, num2 = 0;
    static uint8_t key_flag = 0;

    key_data_struct.key_bit_struct.data_up =     HAL_GPIO_ReadPin(Touch_Key_Up.GPIOx, Touch_Key_Up.GPIO_Pin);
    key_data_struct.key_bit_struct.data_down =   HAL_GPIO_ReadPin(Touch_Key_Down.GPIOx, Touch_Key_Down.GPIO_Pin);
    key_data_struct.key_bit_struct.data_left =   HAL_GPIO_ReadPin(Touch_Key_Left.GPIOx, Touch_Key_Left.GPIO_Pin);
    key_data_struct.key_bit_struct.data_right =  HAL_GPIO_ReadPin(Touch_Key_Right.GPIOx, Touch_Key_Right.GPIO_Pin);
    key_data_struct.key_bit_struct.data_A =      HAL_GPIO_ReadPin(Touch_Key_A.GPIOx, Touch_Key_A.GPIO_Pin);
    key_data_struct.key_bit_struct.data_B =      HAL_GPIO_ReadPin(Touch_Key_B.GPIOx, Touch_Key_B.GPIO_Pin);
    key_data_struct.key_bit_struct.data_C =      HAL_GPIO_ReadPin(Touch_Key_C.GPIOx, Touch_Key_C.GPIO_Pin);
    key_data_struct.key_bit_struct.data_D =      HAL_GPIO_ReadPin(Touch_Key_D.GPIOx, Touch_Key_D.GPIO_Pin);

    if(key_flag == 0)
    {
        if(key_data_struct.key_code != 0xff)
        {
            num1++;
            num2 = 0;
        }
        else
        {
            num1 = 0;
            num2++;
        }
        if(num1 >= 20)
        {
            num1 = 0;
            num2 = 0;
            switch (key_data_struct.key_code)
            {
                case 0xFE: 
                    key_flag = 1;  //标记此次按键有效
                    Touch_Key_Value = TOUCH_KEY_UP;
                    break;
                case 0xFD:
                    key_flag = 1;
                    Touch_Key_Value = TOUCH_KEY_DOWN;
                    break;
                case 0xFB:
                    key_flag = 1;
                    Touch_Key_Value = TOUCH_KEY_LEFT;
                    break;
                case 0xF7:
                    key_flag = 1;
                    Touch_Key_Value = TOUCH_KEY_RIGHT;
                    break;
                case 0xEF:
                    key_flag = 1;
                    Touch_Key_Value = TOUCH_KEY_A;
                    break;
                case 0xDF:
                    key_flag = 1;
                    Touch_Key_Value = TOUCH_KEY_B;
                    break;
                case 0xBF:
                    key_flag = 1;
                    Touch_Key_Value = TOUCH_KEY_C;
                    break;
                case 0x7F:
                    key_flag = 1;
                    Touch_Key_Value = TOUCH_KEY_D;
                    break;
                
                default:
                    break;
            }
        }
    }
    else //松手有效检测
    {
        if(key_data_struct.key_code == 0xff)
        {
            num1++;
            num2 = 0;
        }
        else
        {
            num2++;
            num1 = 0;
        }
        if(num1 >= 20)
        {
            key_flag = 0;
            key_data_struct.key_code = 0xff;
            num1 = 0;
            num2 = 0;
        }
    }
}



