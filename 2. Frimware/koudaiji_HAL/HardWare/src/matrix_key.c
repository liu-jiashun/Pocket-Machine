#include "matrix_key.h"

GPIO_Struct Matrix_Key_Row[5]; 
GPIO_Struct Matrix_Key_List[4];  //5*4���󰴼��к��еĽṹ������

MATRIX_KEY_Enum Matrix_Key_Value;   //��ǰ������ֵ

/*�����󰴼��ӿڳ�ʼ�� ��©�����������stm32�ӿڳ�ʼ��Ϊ��©�������֮���ͬ��׼˫��ڣ�����ʵ�֡����롱���ܣ�*/
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
*���ܣ����󰴼�ɨ��
*����ֵ��������ֵ
*/
void Matrix_Key_Scan(void)
{
    static uint16_t row_data = 0; //�м��������
    static uint16_t list_data = 0; //�м��������
    static uint8_t row_code = 0; //�м�ֵ
    static uint8_t list_code = 0; //�м�ֵ

    static uint8_t key_ok = 0; //�˴ΰ�����Ч��־
    static uint16_t code_data = 0;
    static uint8_t key_code = 0;
    static uint8_t lesson_num = 0;
    
    if(row_data == 0)
    {
        HAL_GPIO_WritePin(Matrix_Key_Row[0].GPIOx, Matrix_Key_Row[0].GPIO_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(Matrix_Key_Row[1].GPIOx, Matrix_Key_Row[1].GPIO_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(Matrix_Key_Row[2].GPIOx, Matrix_Key_Row[2].GPIO_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(Matrix_Key_Row[3].GPIOx, Matrix_Key_Row[3].GPIO_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(Matrix_Key_Row[4].GPIOx, Matrix_Key_Row[4].GPIO_Pin, GPIO_PIN_SET);  //����������͵�ƽ

        HAL_GPIO_WritePin(Matrix_Key_List[0].GPIOx, Matrix_Key_List[0].GPIO_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(Matrix_Key_List[1].GPIOx, Matrix_Key_List[1].GPIO_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(Matrix_Key_List[2].GPIOx, Matrix_Key_List[2].GPIO_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(Matrix_Key_List[3].GPIOx, Matrix_Key_List[3].GPIO_Pin, GPIO_PIN_RESET);  //����������ߵ�ƽ
    }
    
    //��ȡ�����ŵĵ�ƽ�����Ϊ��֤���а������£�����¼������һ��
    //row_data�ӵ�10Ϊ����
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
    
    //���������м�����֮���ټ����
    else
    {
		if(list_data == 0)
		{
			HAL_GPIO_WritePin(Matrix_Key_Row[0].GPIOx, Matrix_Key_Row[0].GPIO_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Matrix_Key_Row[1].GPIOx, Matrix_Key_Row[1].GPIO_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Matrix_Key_Row[2].GPIOx, Matrix_Key_Row[2].GPIO_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Matrix_Key_Row[3].GPIOx, Matrix_Key_Row[3].GPIO_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(Matrix_Key_Row[4].GPIOx, Matrix_Key_Row[4].GPIO_Pin, GPIO_PIN_RESET); //������͵�ƽ

			HAL_GPIO_WritePin(Matrix_Key_List[0].GPIOx, Matrix_Key_List[0].GPIO_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(Matrix_Key_List[1].GPIOx, Matrix_Key_List[1].GPIO_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(Matrix_Key_List[2].GPIOx, Matrix_Key_List[2].GPIO_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(Matrix_Key_List[3].GPIOx, Matrix_Key_List[3].GPIO_Pin, GPIO_PIN_SET); //������ߵ�ƽ
		}
        //��ⰴ�������У�����¼������ ͬ��������
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
        //���ݼ�¼�����������������ֵ������ǰ�����Ч
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

    if(key_ok == 1) //������Ч֮��ȴ�����
    {
        if(code_data == 0) //����һֱ����ʱ���������е��˴�code_data����Ϊ0
        {
            lesson_num ++;
            if(lesson_num >= 100)
            {
                Matrix_Key_Value = (MATRIX_KEY_Enum)key_code; //����֮�����ո�ֵ��ֵ
                lesson_num = 0;
                key_ok = 0;
            }
        }
    }
    code_data = 0; //����code_data
}




