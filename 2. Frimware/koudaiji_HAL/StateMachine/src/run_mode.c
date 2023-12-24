#include "run_mode.h"
#include "lcd.h"

Mode_Struct Run_Mode_Stop, Run_Mode_Static, Run_Mode_Slow, Run_Mode_Fast, Run_Mode_Manual; //运行模式的结构体变量（停止模式，静态模式，慢速模式，快速模式，手动模式）
Mode_Struct *Run_Mode_Now;  //当前运行模式的指针

bool enter_voice_flag = true;

/*运行模式初始化*/
void Run_Mode_Init(void)
{
    Run_Mode_Stop_Init();
    Run_Mode_Static_Init();
    Run_Mode_Slow_Init();
    Run_Mode_Fast_Init();
    Run_Mode_Manual_Init();

    Run_Mode_Now  = &Run_Mode_Stop; //初始状态下运行模式默认为停止模式
}



