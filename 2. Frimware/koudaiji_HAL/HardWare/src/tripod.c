#include "tripod.h"
#include <math.h>
#include "tim.h"
#include "core.h"

GPIO_Struct Tripod_X_DIR, Tripod_X_EN, Tripod_X_STEP, Tripod_X_LIMIT, Tripod_Y_DIR, Tripod_Y_EN, Tripod_Y_STEP, Tripod_Y_LIMIT; // 云台控制端口结构体
Tripod_Struct Tripod_X, Tripod_Y;                                                                                               // 云台相关结构体

uint16_t Y_OFFSET = 200; // 校准完成后激光照射在Y轴上的坐标 /系统以左下角为原点

/*云台初始化*/
void Tripod_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    Tripod_X_DIR.GPIOx = GPIOD;
    Tripod_X_DIR.GPIO_Pin = GPIO_PIN_1; // D1 -> X_DIR
    Tripod_X_EN.GPIOx = GPIOA;
    Tripod_X_EN.GPIO_Pin = GPIO_PIN_15; // A15 -> X_EN
    Tripod_X_STEP.GPIOx = GPIOG;
    Tripod_X_STEP.GPIO_Pin = GPIO_PIN_15; // G15 -> X_STEP
    Tripod_X_LIMIT.GPIOx = GPIOB;
    Tripod_X_LIMIT.GPIO_Pin = GPIO_PIN_6; // B6 -> X限位信号读取
    Tripod_Y_DIR.GPIOx = GPIOE;
    Tripod_Y_DIR.GPIO_Pin = GPIO_PIN_0; // E0 -> Y_DIR
    Tripod_Y_EN.GPIOx = GPIOE;
    Tripod_Y_EN.GPIO_Pin = GPIO_PIN_7; // E7 -> Y_EN
    Tripod_Y_STEP.GPIOx = GPIOE;
    Tripod_Y_STEP.GPIO_Pin = GPIO_PIN_9; // E9 -> Y_STEP
    Tripod_Y_LIMIT.GPIOx = GPIOE;
    Tripod_Y_LIMIT.GPIO_Pin = GPIO_PIN_11; // E11 -> Y限位信号读取

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_7 | GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    HAL_GPIO_WritePin(Tripod_X_DIR.GPIOx, Tripod_X_DIR.GPIO_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Tripod_X_EN.GPIOx, Tripod_X_EN.GPIO_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Tripod_X_STEP.GPIOx, Tripod_X_STEP.GPIO_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Tripod_Y_DIR.GPIOx, Tripod_Y_DIR.GPIO_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Tripod_Y_EN.GPIOx, Tripod_Y_EN.GPIO_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(Tripod_Y_STEP.GPIOx, Tripod_Y_STEP.GPIO_Pin, GPIO_PIN_RESET);

    Tripod_X.angle = 0.0;
    Tripod_X.coord = 684;
    Tripod_X.is_running = false;
    Tripod_X.is_limit = false;
    Tripod_X.step_num = 0;
    Tripod_Y.angle = 0.0;
    Tripod_Y.coord = Y_OFFSET;
    Tripod_Y.is_running = false;
    Tripod_Y.is_limit = false;
    Tripod_Y.step_num = 0;
}

/*设置X轴运动方向*/
void Tripod_X_Dir(uint8_t dir) // 0 左，1 右
{
    if (dir == 1)
    {
        HAL_GPIO_WritePin(Tripod_X_DIR.GPIOx, Tripod_X_DIR.GPIO_Pin, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(Tripod_X_DIR.GPIOx, Tripod_X_DIR.GPIO_Pin, GPIO_PIN_RESET);
    }
    HAL_Delay(10);
}

/*设置Y轴运动方向*/
void Tripod_Y_Dir(uint8_t dir) // 0 上， 1 下
{
    if (dir == 1)
    {
        HAL_GPIO_WritePin(Tripod_Y_DIR.GPIOx, Tripod_Y_DIR.GPIO_Pin, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(Tripod_Y_DIR.GPIOx, Tripod_Y_DIR.GPIO_Pin, GPIO_PIN_SET);
    }
    HAL_Delay(10);
}

/*设置X轴步进步数*/
void Tripod_X_Step(uint16_t num)
{
    if (num == 0)
        return;
    HAL_GPIO_WritePin(Tripod_X_STEP.GPIOx, Tripod_X_STEP.GPIO_Pin, GPIO_PIN_RESET);
    Tripod_X.step_num = num * 2;
    Tripod_X.step_code = Tripod_X.step_num;
    Tripod_X.is_running = true;
    __HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);
    __HAL_TIM_SET_PRESCALER(&htim2, 84 * 5);
    HAL_TIM_Base_Start_IT(&htim2);
}

/*设置Y轴步进步数*/
void Tripod_Y_Step(uint16_t num)
{
    if (num == 0)
        return;
    HAL_GPIO_WritePin(Tripod_Y_STEP.GPIOx, Tripod_Y_STEP.GPIO_Pin, GPIO_PIN_RESET);
    Tripod_Y.step_num = num * 2;
    Tripod_Y.step_code = Tripod_Y.step_num;
    Tripod_Y.is_running = true;
    __HAL_TIM_CLEAR_IT(&htim4, TIM_IT_UPDATE);
    __HAL_TIM_SET_PRESCALER(&htim4, 84 * 5);
    HAL_TIM_Base_Start_IT(&htim4);
}

/*云台两轴是否到达限位扫描*/
void Tripod_Key_Scan(void)
{
    static uint8_t x_num1, x_num2, y_num1, y_num2;
    if (HAL_GPIO_ReadPin(Tripod_X_LIMIT.GPIOx, Tripod_X_LIMIT.GPIO_Pin) == GPIO_PIN_RESET)
    {
        x_num1++;
        x_num2 = 0;
    }
    else
    {
        x_num1 = 0;
        x_num2++;
    }
    if (x_num1 > 50)
    {
        Tripod_X.is_limit = true;
        x_num1 = 0;
    }
    if (x_num2 > 50)
    {
        x_num2 = 0;
        Tripod_X.is_limit = false;
    }

    if (HAL_GPIO_ReadPin(Tripod_Y_LIMIT.GPIOx, Tripod_Y_LIMIT.GPIO_Pin) == GPIO_PIN_RESET)
    {
        y_num1++;
        y_num2 = 0;
    }
    else
    {
        y_num1 = 0;
        y_num2++;
    }
    if (y_num1 > 50)
    {
        Tripod_Y.is_limit = true;
        y_num1 = 0;
    }
    if (y_num2 > 50)
    {
        y_num2 = 0;
        Tripod_Y.is_limit = false;
    }
}

/*云台X轴机械归零*/
void Tripod_X_Go_Mechzero(void)
{
    if (Tripod_X.is_limit == false)
    {
        Tripod_X_Dir(1);
        Tripod_X_Step(0xffff);
        while (!Tripod_X.is_limit)
            ;
        HAL_TIM_Base_Stop(&htim2);
        __HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);
        HAL_Delay(500);
        Tripod_X_Step(200);
        while (Tripod_X.is_running)
            ;
    }
    if (Tripod_X.is_limit == true)
    {
        Tripod_X_Dir(0);
        Tripod_X_Step(0xffff);
        while (Tripod_X.is_limit)
            ;
        HAL_TIM_Base_Stop(&htim2);
        __HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE);
        __HAL_TIM_SET_PRESCALER(&htim2, 84);
    }

    HAL_Delay(500);
    Tripod_X_Dir(1);
    Tripod_X_Step(100);
    while (Tripod_X.is_running)
        ;

    Tripod_X.is_running = false;
}

/*云台Y轴机械归零*/
void Tripod_Y_Go_Mechzero(void)
{
    if (Tripod_Y.is_limit == true)
    {
        Tripod_Y_Dir(1);
        Tripod_Y_Step(0xffff);
        while (Tripod_Y.is_limit)
            ;
        HAL_TIM_Base_Stop(&htim4);
        __HAL_TIM_CLEAR_IT(&htim4, TIM_IT_UPDATE);
        HAL_Delay(500);
        Tripod_Y_Step(200);
        while (Tripod_Y.is_running)
            ;
    }
    if (Tripod_Y.is_limit == false)
    {
        Tripod_Y_Dir(0);
        Tripod_Y_Step(0xffff);
        while (!Tripod_Y.is_limit)
            ;
        HAL_TIM_Base_Stop(&htim4);
        __HAL_TIM_CLEAR_IT(&htim4, TIM_IT_UPDATE);
        __HAL_TIM_SET_PRESCALER(&htim4, 84);
    }

    HAL_Delay(500);
    Tripod_Y_Dir(1);
    Tripod_Y_Step(180);
    while (Tripod_Y.is_running)
        ;

    Tripod_Y.is_running = false;
}

/*设置云台X轴角度*/
void Tripod_X_Angle(float x_dat)
{
    uint16_t step;
    if (x_dat >= Tripod_X.angle)
    {
        Tripod_X_Dir(1);
        step = (x_dat - Tripod_X.angle) * X_STEP_ANGLE;
        Tripod_X_Step(step);
    }
    else
    {
        Tripod_X_Dir(0);
        step = (Tripod_X.angle - x_dat) * X_STEP_ANGLE;
        Tripod_X_Step(step);
    }

    Tripod_X.angle = x_dat;
}

/*设置云台Y轴角度*/
void Tripod_Y_Angle(float y_dat)
{
    uint16_t step;
    if (y_dat >= Tripod_Y.angle)
    {
        Tripod_Y_Dir(1);
        step = (y_dat - Tripod_Y.angle) * Y_STEP_ANGLE;
        Tripod_Y_Step(step);
    }
    else
    {
        Tripod_Y_Dir(0);
        step = (Tripod_Y.angle - y_dat) * Y_STEP_ANGLE;
        Tripod_Y_Step(step);
    }
    Tripod_Y.angle = y_dat;
}

// x轴坐标动作   到达指定坐标
void Tripod_X_Coord(int x_dat)
{
    float temp;

    while (Tripod_X.is_running)
        ;
    temp = (float)x_dat;
    temp = temp - 684;
    temp = atan2(temp, DIS_X);
    temp = temp * 57.29577f; // 弧度转角度

    Tripod_X_Angle(temp);
    Tripod_X.coord = x_dat;
}

// y坐标动作  到达指定坐标
void Tripod_Y_Coord(int y_dat)
{
    float temp;
    float x_len;

    while (Tripod_Y.is_running)
        ;
    temp = (float)y_dat;                          //
    temp = Y_OFFSET - temp;                       //
    x_len = hypot((Tripod_X.coord - 684), DIS_X); // 旋转角度矫正
    temp = atan2(temp, x_len);
    temp = temp * 57.29577f;

    Tripod_Y_Angle(temp);
    Tripod_Y.coord = y_dat;
}
