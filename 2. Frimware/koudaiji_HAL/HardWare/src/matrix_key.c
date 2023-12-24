#include "matrix_key.h"

GPIO_Struct Matrix_Key_Row[5]; 
GPIO_Struct Matrix_Key_List[4];  //5*4矩阵按键行和列的结构体数组

MATRIX_KEY_Enum Matrix_Key_Value;   //当前按键键值

/*将矩阵按键接口初始化 开漏输出，上拉（stm32接口初始化为开漏上拉输出之后等同于准双向口，可以实现“线与”功能）*/
void Matrix_Key_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    Matrix_Key_Row[0].GPIOx = GPIOF;   Matrix_Key_Row[0].GPIO_Pin = GPIO_PIN_1;
    Matrix_Key_Row[1].GPIOx = GPIOF;   Matrix_Key_Row[1].GPIO_Pin = GPIO_PIN_12;
    Matrix_Key_Row[2].GPIOx = GPIOD;   Matrix_Key_Row[2].GPIO_Pin = GPIO_PIN_12;
    Matrix_Key_Row[3].GPIOx = GPIOF;   Matrix_Key_Row[3].GPIO_Pin = GPIO_PIN_13;
    Matrix_Key_Row[4].GPIOx = GPIOD;   Matrix_Key_Row[4].GPIO_Pin = GPIO_PIN_14;
    Matrix_Key_List[0].GPIOx = GPIOD;  Matrix_Key_List[0].GPIO_Pin = GPIO_PIN_9;
    Matrix_Key_List[1].GPIOx = GPIOB;  Matrix_Key_List[1].GPIO_Pin = GPIO_PIN_1;
    Matrix_Key_List[2].GPIOx = GPIOF;  Matrix_Key_List[2].GPIO_Pin = GPIO_PIN_3;
    Matrix_Key_List[3].GPIOx = GPIOF;  Matrix_Key_List[3].GPIO_Pin = GPIO_PIN_11;

    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_12 | GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/*
*功能：矩阵按键扫描
*返回值：按键键值
*/
void Matrix_Key_Scan(void)
{
    static uint16_t row_data = 0; //行检测消抖用
    static uint16_t list_data = 0; //列检测消抖用
    static uint8_t row_code = 0; //行键值
    static uint8_t list_code = 0; //列键值

    static uint8_t key_ok = 0; //此次按键有效标志
    static uint16_t code_data = 0;
    static uint8_t key_code = 0;
    static uint8_t lesson_num = 0;
    
    if(row_data == 0)
    {
        HAL_GPIO_WritePin(Matrix_Key_Row[0].GPIOx, Matrix_Key_Row[0].GPIO_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(Matrix_Key_Row[1].GPIOx, Matrix_Key_Row[1].GPIO_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(Matrix_Key_Row[2].GPIOx, Matrix_Key_Row[2].GPIO_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(Matrix_Key_Row[3].GPIOx, Matrix_Key_Row[3].GPIO_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(Matrix_Key_Row[4].GPIOx, Matrix_Key_Row[4].GPIO_Pin, GPIO_PIN_SET);  //行引脚输出低电平

        HAL_GPIO_WritePin(Matrix_Key_List[0].GPIOx, Matrix_Key_List[0].GPIO_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(Matrix_Key_List[1].GPIOx, Matrix_Key_List[1].GPIO_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(Matrix_Key_List[2].GPIOx, Matrix_Key_List[2].GPIO_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(Matrix_Key_List[3].GPIOx, Matrix_Key_List[3].GPIO_Pin, GPIO_PIN_RESET);  //列引脚输出高电平
    }
    
    //读取行引脚的电平，如果为低证明有按键按下，并记录下在哪一行
    //row_data加到10为消抖
    if(row_data <= 10)
    {
        if(HAL_GPIO_ReadPin(Matrix_Key_Row[0].GPIOx, Matrix_Key_Row[0].GPIO_Pin) == GPIO_PIN_RESET)
        {
            row_data++;
            row_code = 0;
        }
        else if(HAL_GPIO_ReadPin(Matrix_Key_Row[1].GPIOx, Matrix_Key_Row[1].GPIO_Pin) == GPIO_PIN_RESET)
        {
            row_data++;
            row_code = 1;
        }
        else if(HAL_GPIO_ReadPin(Matrix_Key_Row[2].GPIOx, Matrix_Key_Row[2].GPIO_Pin) == GPIO_PIN_RESET)
        {
            row_data++;
            row_code = 2; 
        }
        else if(HAL_GPIO_ReadPin(Matrix_Key_Row[3].GPIOx, Matrix_Key_Row[3].GPIO_Pin) == GPIO_PIN_RESET)
        {
            row_data++;
            row_code = 3; 
        }
        else if(HAL_GPIO_ReadPin(Matrix_Key_Row[4].GPIOx, Matrix_Key_Row[4].GPIO_Pin) == GPIO_PIN_RESET)
        {
            row_data++;
            row_code = 4; 
        }
        else
        {
            row_code = 0;
            row_data = 0;
            list_code = 0;
            list_data = 0;
        }
    }
    
    //按键所在行检测出来之后再检测列
    else
    {
		if(list_data == 0)
		{
			HAL_GPIO_WritePin(Matrix_Key_Row[0].GPIOx, Matrix_Key_Row[0].GPIO_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Matrix_Key_Row[1].GPIOx, Matrix_Key_Row[1].GPIO_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Matrix_Key_Row[2].GPIOx, Matrix_Key_Row[2].GPIO_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Matrix_Key_Row[3].GPIOx, Matrix_Key_Row[3].GPIO_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Matrix_Key_Row[4].GPIOx, Matrix_Key_Row[4].GPIO_Pin, GPIO_PIN_RESET); //行输出低电平

			HAL_GPIO_WritePin(Matrix_Key_List[0].GPIOx, Matrix_Key_List[0].GPIO_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(Matrix_Key_List[1].GPIOx, Matrix_Key_List[1].GPIO_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(Matrix_Key_List[2].GPIOx, Matrix_Key_List[2].GPIO_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(Matrix_Key_List[3].GPIOx, Matrix_Key_List[3].GPIO_Pin, GPIO_PIN_SET); //列输出高电平
		}
        //检测按键所在列，并记录下列数 同样有消抖
        if(list_data <= 10)
        {
            if(HAL_GPIO_ReadPin(Matrix_Key_List[0].GPIOx, Matrix_Key_List[0].GPIO_Pin) == GPIO_PIN_RESET)
            {
                list_data++;
                list_code = 0;
            }
            else if(HAL_GPIO_ReadPin(Matrix_Key_List[1].GPIOx, Matrix_Key_List[1].GPIO_Pin) == GPIO_PIN_RESET)
            {
                list_data++;
                list_code = 1;
            } 
            else if(HAL_GPIO_ReadPin(Matrix_Key_List[2].GPIOx, Matrix_Key_List[2].GPIO_Pin) == GPIO_PIN_RESET) 
            {
                list_data++;
                list_code = 2;
            }
            else if(HAL_GPIO_ReadPin(Matrix_Key_List[3].GPIOx, Matrix_Key_List[3].GPIO_Pin) == GPIO_PIN_RESET) 
            {
                list_data++;
                list_code = 3;
            }
            else
            {
                list_data = 0;
                list_code = 0;
                row_data = 0;
                row_code = 0;
            }
        }
        //根据记录的行数和列数计算键值，并标记按键有效
        else
        {
            code_data = row_code*4 + list_code + 1;
            key_ok = 1;
            lesson_num = 0;
            key_code = code_data;
            row_data = 0;
            list_data = 0;
            list_code = 0;
            row_code = 0;
        }
    }

    if(key_ok == 1) //按键有效之后等待松手
    {
        if(code_data == 0) //按键一直按着时，程序运行到此处code_data不会为0
        {
            lesson_num ++;
            if(lesson_num >= 100)
            {
                Matrix_Key_Value = (MATRIX_KEY_Enum)key_code; //松手之后最终赋值键值
                lesson_num = 0;
                key_ok = 0;
            }
        }
    }
    code_data = 0; //清零code_data
}




