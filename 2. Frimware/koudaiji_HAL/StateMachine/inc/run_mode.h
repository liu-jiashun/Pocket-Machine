#ifndef __RUN_MODE_H__
#define __RUN_MODE_H__

#include "run_stop.h"
#include "run_static.h"
#include "run_slow.h"
#include "run_fast.h"
#include "run_manual.h"
#include "logic.h"

typedef struct _Mode_Struct //运行模式相关结构体，主要是三个函数指针（模式进入函数，模式运行函数，模式退出函数）
{
    void (*mode_enter)(void);
    void (*mode_run)(void);
    void (*mode_exit)(void);
}Mode_Struct;

extern Mode_Struct *Run_Mode_Now;
extern Mode_Struct Run_Mode_Stop, Run_Mode_Static, Run_Mode_Slow, Run_Mode_Fast, Run_Mode_Manual;

extern bool enter_voice_flag;

void Run_Mode_Init(void);

#endif



