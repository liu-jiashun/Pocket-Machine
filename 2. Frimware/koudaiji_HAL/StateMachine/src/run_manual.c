#include "run_manual.h"
#include "run_mode.h"
#include "lcd.h"
#include "voice.h"
#include "matrix_key.h"
#include "diwen_lcd.h"
#include "lcd.h"
#include "tripod.h"
#include "relay.h"

Run_Manual_Struct Run_Manual;

/*手动模式初始化，主要是将函数指针指向各个实现函数，并将一些变量的值清0*/
void Run_Mode_Manual_Init(void)
{
    Run_Mode_Manual.mode_enter = Run_Mode_Manual_Enter;
    Run_Mode_Manual.mode_exit = Run_Mode_Manual_Exit;
    Run_Mode_Manual.mode_run = Run_Mode_Manual_Run;

    Run_Manual.step = 0;
    Run_Manual.mode = 0;
    Run_Manual.target_x = 0;
    Run_Manual.target_y = 0;

    Run_Manual.res_sum = 0;
}

/*手动模式进入函数*/
void Run_Mode_Manual_Enter(void)
{
    Run_Manual.step = 0;
    Run_Manual.mode = 0;
    Run_Manual.target_x = 0;
    Run_Manual.target_y = 0;  //清零变量
    DiWenLcd_Show_RunState(129, "手动模式");

    Voice_Play((uint8_t)MANUAL_START); //播放语音
    HAL_Delay(600);
    while(voice_is_busy);  //等待语音播放完毕
    Relay_Control(RELAY_ON); //打开激光笔（手动模式下，进入的时候直接打开激光笔，退出的时候关闭）
    Lcd_printf("coord mode");
}

/*手动模式退出函数*/
void Run_Mode_Manual_Exit(void)
{
    Relay_Control(RELAY_OFF); //关闭激光笔
}

/*手动模式运行函数*/
void Run_Mode_Manual_Run(void)
{
    //“上”，“下”，“左”，“右”按键微调功能
    if(Matrix_Key_Value != MATRIX_KEY_NONE)
    {
        switch (Matrix_Key_Value)
        {
            case MATRIX_KEY_UP:
                Tripod_Y_Coord(Tripod_Y.coord - 10);
                Matrix_Key_Value = MATRIX_KEY_NONE;
            break;
            case MATRIX_KEY_DOWN:
                Tripod_Y_Coord(Tripod_Y.coord + 10);
                Matrix_Key_Value = MATRIX_KEY_NONE;
            break;
            case MATRIX_KEY_LEFT:
                Tripod_X_Coord(Tripod_X.coord - 10);
                Matrix_Key_Value = MATRIX_KEY_NONE;
            break;
            case MATRIX_KEY_RIGHT:
                Tripod_X_Coord(Tripod_X.coord + 10);
                Matrix_Key_Value = MATRIX_KEY_NONE;
            break;
            
            default:
            break;
        }
    }

    if(Run_Manual.step == 0)  //第一步，播放语音和屏幕显示提示输入坐标
    {
        Voice_Play((uint8_t)PLASE_INPUT_TARGET_X);
        DiWenLcd_Printf("请输入X坐标");
        Lcd_printf("plase input x");
        Run_Manual.step = 1;
    }
    if(Run_Manual.step == 1) //第二步，检测X坐标输入
    {
        if(Matrix_Key_Value != MATRIX_KEY_NONE)
        {
            if(Matrix_Key_Value == MATRIX_KEY_0 || Matrix_Key_Value == MATRIX_KEY_1 || Matrix_Key_Value == MATRIX_KEY_2 || Matrix_Key_Value == MATRIX_KEY_3 || \
               Matrix_Key_Value == MATRIX_KEY_4 || Matrix_Key_Value == MATRIX_KEY_5 || Matrix_Key_Value == MATRIX_KEY_6 || Matrix_Key_Value == MATRIX_KEY_7 || \
               Matrix_Key_Value == MATRIX_KEY_8 || Matrix_Key_Value == MATRIX_KEY_9)
            {
                uint8_t num;
                switch (Matrix_Key_Value)
                {
                case MATRIX_KEY_0:
                    num = 0;
                    break;
                case MATRIX_KEY_1:
                    num = 1;
                    break;
                case MATRIX_KEY_2:
                    num = 2;
                    break;
                case MATRIX_KEY_3:
                    num = 3;
                    break;
                case MATRIX_KEY_4:
                    num = 4;
                    break;
                case MATRIX_KEY_5:
                    num = 5;
                    break;
                case MATRIX_KEY_6:
                    num = 6;
                    break;
                case MATRIX_KEY_7:
                    num = 7;
                    break;
                case MATRIX_KEY_8:
                    num = 8;
                    break;
                case MATRIX_KEY_9:
                    num = 9;
                    break;
                
                default:
                    break;
                }
                Run_Manual.target_x = Run_Manual.target_x*10 + num;

                if(Run_Manual.mode == 0)
                {
                    if(Run_Manual.target_x > 1365)  //超出范围提示输入错误
                    {
                        Voice_Play((uint8_t)INPUT_ERROR);
                        DiWenLcd_Printf("输入错误");
                        Lcd_printf("input error");
                        Run_Manual.target_x = 0;
                    }
                }
                else
                {
                    if(Run_Manual.target_x > 70)  //超出范围提示输入错误
                    {
                        Voice_Play((uint8_t)INPUT_ERROR);
                        DiWenLcd_Printf("输入错误");
                        Lcd_printf("input error");
                        Run_Manual.target_x = 0;
                    }
                }
                
                DiWenLcd_Cmd_Data(0x600A, Run_Manual.target_x);

                Matrix_Key_Value = MATRIX_KEY_NONE;
            }

            if(Matrix_Key_Value == MATRIX_KEY_D)
            {
                Voice_Play((uint8_t)PLASE_INPUT_TARGET_Y);
                DiWenLcd_Printf("请输入Y坐标");  //X坐标输入正确提示输入Y坐标
                Lcd_printf("plase input y");
                Run_Manual.step = 2;

                Matrix_Key_Value = MATRIX_KEY_NONE;
            }
        }
    }
    if(Run_Manual.step == 2)  //第三步，检测Y坐标输入
    {
        if(Matrix_Key_Value != MATRIX_KEY_NONE)
        {
            if(Matrix_Key_Value == MATRIX_KEY_0 || Matrix_Key_Value == MATRIX_KEY_1 || Matrix_Key_Value == MATRIX_KEY_2 || Matrix_Key_Value == MATRIX_KEY_3 || \
               Matrix_Key_Value == MATRIX_KEY_4 || Matrix_Key_Value == MATRIX_KEY_5 || Matrix_Key_Value == MATRIX_KEY_6 || Matrix_Key_Value == MATRIX_KEY_7 || \
               Matrix_Key_Value == MATRIX_KEY_8 || Matrix_Key_Value == MATRIX_KEY_9)
            {
                uint8_t num;
                switch (Matrix_Key_Value)
                {
                case MATRIX_KEY_0:
                    num = 0;
                    break;
                case MATRIX_KEY_1:
                    num = 1;
                    break;
                case MATRIX_KEY_2:
                    num = 2;
                    break;
                case MATRIX_KEY_3:
                    num = 3;
                    break;
                case MATRIX_KEY_4:
                    num = 4;
                    break;
                case MATRIX_KEY_5:
                    num = 5;
                    break;
                case MATRIX_KEY_6:
                    num = 6;
                    break;
                case MATRIX_KEY_7:
                    num = 7;
                    break;
                case MATRIX_KEY_8:
                    num = 8;
                    break;
                case MATRIX_KEY_9:
                    num = 9;
                    break;
                
                default:
                    break;
                }
                Run_Manual.target_y = Run_Manual.target_y*10 + num;

                if(Run_Manual.mode == 0)
                {
                   if(Run_Manual.target_y > (DIS_Y-1))
                    {
                        Voice_Play((uint8_t)INPUT_ERROR);
                        DiWenLcd_Printf("输入错误");  //超过范围提示输入错误
                        Lcd_printf("input error");
                        Run_Manual.target_y = 0;
                    } 
                }
                else
                {
                    if(Run_Manual.target_y > 39)
                    {
                        Voice_Play((uint8_t)INPUT_ERROR);
                        DiWenLcd_Printf("输入错误");  //超过范围提示输入错误
                        Lcd_printf("input error");
                        Run_Manual.target_y = 0;
                    } 
                }
                
                DiWenLcd_Cmd_Data(0x600C, Run_Manual.target_y);

                Matrix_Key_Value = MATRIX_KEY_NONE;
            }

            if(Matrix_Key_Value == MATRIX_KEY_D)
            {
                Voice_Play((uint8_t)LOCATION_TARGET_CORRD);
                Run_Manual.res_sum++;
				DiWenLcd_Cmd_Data(0x6008, Run_Manual.res_sum);
                Run_Manual.step = 0;

                if(Run_Manual.mode == 0)  //坐标模式
                {
                    DiWenLcd_Printf("指定坐标 X:%d, Y:%d", Run_Manual.target_x, Run_Manual.target_y);
                    Lcd_printf("Manual Corrd x:%d, y:%d", Run_Manual.target_x, Run_Manual.target_y);
                    Tripod_X_Coord(Run_Manual.target_x);
                    Tripod_Y_Coord(Run_Manual.target_y); //X、Y坐标均输入正确，云台运动到指定坐标
                }
                else  //距离模式
                {
                    DiWenLcd_Printf("指定距离 X:%d, Y:%d", Run_Manual.target_x, Run_Manual.target_y);
                    Lcd_printf("Manual Dist x:%d, y:%d", Run_Manual.target_x, Run_Manual.target_y);
                    Run_Manual.target_x = Run_Manual.target_x * 19.5143f;
                    Run_Manual.target_y = Run_Manual.target_y * 19.443f;  //距离转成坐标
                    Tripod_X_Coord(Run_Manual.target_x);
                    Tripod_Y_Coord(Run_Manual.target_y); //X、Y坐标均输入正确，云台运动到指定坐标
                }
                
                Run_Manual.target_x = 0;
                Run_Manual.target_y = 0;
                HAL_Delay(600);
                while(voice_is_busy); //等待语音播报完成

                Matrix_Key_Value = MATRIX_KEY_NONE;
            }
        }
    }
}


