#include "run_static.h"
#include "run_mode.h"
#include "lcd.h"
#include "voice.h"
#include "infared.h"
#include "tripod.h"
#include "relay.h"
#include "diwen_lcd.h"
#include <math.h>
#include <stdlib.h>

Run_Static_Struct Run_Static;

/*静态模式初始化，主要是将函数指针指向各个实现函数，并将一些变量的值清0*/
void Run_Mode_Static_Init(void)
{
    Run_Mode_Static.mode_enter = Run_Mode_Static_Enter;
    Run_Mode_Static.mode_exit = Run_Mode_Static_Exit;
    Run_Mode_Static.mode_run = Run_Mode_Static_Run;

    Run_Static.step = 0;
    Run_Static.res_num = 0;
    Run_Static.res_sum = 0;

    Run_Static.code_corrd_x = 0;
    Run_Static.point_num = 0;
    Run_Static.same_flag = false;
    Run_Static.user_num = 0;
}
 

/*静态模式进入函数*/
void Run_Mode_Static_Enter(void)
{
    Run_Static.step = 0;
    Run_Static.res_num = 0;
    Run_Static.code_corrd_x = 0;
    Run_Static.point_num = 0;
    Run_Static.same_flag = false;
    Run_Static.user_num = 0;

    Infared.Infared_Is_Pause = false;

    DiWenLcd_Show_RunState(129, "静态模式"); //迪文屏显示运行模式

    /*因为切换检测物体的形状和颜色的时候也会调用一次模式进入函数，此时不需要播放进入语音*/
    if(enter_voice_flag == true)  //如果允许播放语音
    {
        Voice_Play((uint8_t)STATIC_START);  //播放进入慢速模式的语音
    }
    else  //如果此次不允许播放语音
    {
        enter_voice_flag = true;  //清空该标志位
    }
}

/*静态模式退出函数*/
void Run_Mode_Static_Exit(void)
{
}

/*静态模式运行函数*/
void Run_Mode_Static_Run(void)
{
    if(Run_Static.step == 0)  //第一步，轮询AI模组是否检测到指定物体
    {
        if(Vision_Dat_Explan() != 0)  //AI模组检测到物体
        {
            if(abs(Run_Static.code_corrd_x - Vision_Coord.Res_Coord[Vision_Coord.Res_Num-1].x)<STATIC_ALLOW)  //如果当前暂存X坐标和识别到的坐标偏差在允许的范围内
            {
                Run_Static.user_num++; //计数+1
                if(Run_Static.user_num >= 5) //计数到3任务图像已经稳定，进入下一步（等待图像坐标稳定是因为电视机切换图片的瞬间图像的颜色和形状都会失真）
                {
                    Run_Static.step = 1;
                    Run_Static.user_num = 0;
                    Run_Static.code_corrd_x = 0;
                } 
            }
            else  //超过允许范围
            {
                Run_Static.user_num = 0; //计数清零
                Run_Static.code_corrd_x = Vision_Coord.Res_Coord[Vision_Coord.Res_Num-1].x; //暂存X坐标=当前检测到的物体坐标
            }
        }
    }

    if(Run_Static.step == 1)  //第二步，判断和上一帧是否是同一个图片（将当前收到的最后一个坐标和保存的上一帧最后一个物体的坐标做对比）
    {
        for(Run_Static.user_num=0; Run_Static.user_num<Vision_Coord.Res_Num; Run_Static.user_num++)
        {
            //检查和上一帧是否是同一个图片（如果两帧的X和Y坐标都相差很小，则认为是同一幅图片）
            Run_Static.same_flag = true;
            if((abs(Run_Static.res_coord[Run_Static.res_num-1].x - Vision_Coord.Res_Coord[Vision_Coord.Res_Num-1].x) > STATIC_ALLOW) || \
                (abs(Run_Static.res_coord[Run_Static.res_num-1].y - Vision_Coord.Res_Coord[Vision_Coord.Res_Num-1].y) > STATIC_ALLOW))
            {
                Run_Static.same_flag = false;
            }
        }
        if(Run_Static.same_flag == true) //如果是同一幅图片
        {
            Run_Static.step = 0; //返回第一步
        }
        else //如果不是同一幅图片
        {
            for(Run_Static.user_num=0; Run_Static.user_num<Vision_Coord.Res_Num; Run_Static.user_num++)
            {
                Run_Static.res_coord[Run_Static.user_num].x = Vision_Coord.Res_Coord[Run_Static.user_num].x;
                Run_Static.res_coord[Run_Static.user_num].y = Vision_Coord.Res_Coord[Run_Static.user_num].y; //取出所有物体的坐标
            }
            Run_Static.res_num = Vision_Coord.Res_Num;
            Run_Static.user_num = 0;
            Run_Static.step = 2; //进入第三步
            Infared_Send_OK();  //红外发射信号暂停电视机      
        }
    }

    if(Run_Static.step == 2)  //第三步，根据检测到的物品的数量挨个指向物体 
    {
        if((Run_Static.user_num % 5000 == 0) && (Run_Static.point_num < Run_Static.res_num)) //5秒一个，挨个指向物体
        {
            Relay_Control(RELAY_OFF); //指示之前关闭激光笔
            Voice_Play((select_shape-1)*4 + select_color); //播放语音
            Tripod_X_Coord(Run_Static.res_coord[Run_Static.point_num].x);
            Tripod_Y_Coord(Run_Static.res_coord[Run_Static.point_num].y); //控制云台X和Y轴转动
            while(Tripod_Y.is_running);
            while(Tripod_X.is_running); //等待转动结束
            Relay_Control(RELAY_ON); //打开激光笔
            //屏幕显示检测结果
            Lcd_printf("Detected %s %s:x=%d,y=%d", ColorStr[select_color-1], ShapeStr[select_shape-1],  Run_Static.res_coord[Run_Static.point_num].x, Run_Static.res_coord[Run_Static.point_num].y);
            DiWenLcd_Printf("检测到%s%s: X=%d,Y=%d", ColorStrZh[select_color-1], ShapeStrZh[select_shape-1], Run_Static.res_coord[Run_Static.point_num].x, Run_Static.res_coord[Run_Static.point_num].y);
            Run_Static.res_sum++; //静态模式下检测到物体总数的计数+1
			DiWenLcd_Cmd_Data(0x6002, Run_Static.res_sum); //向迪文屏发送计数总数
            Run_Static.point_num++;
            Run_Static.user_num = 0;
        }

        Run_Static.user_num++;

        if(Run_Static.user_num == 5001) //程序运行到此处说明上一个判断的(Run_Static.point_num < Run_Static.res_num)条件已经不满足，即所有物体已经指示完
        {
            Relay_Control(RELAY_OFF); //首先关闭激光笔（因为激光笔的照射会干扰AI模组对物体的识别）
        }

        if(Run_Static.user_num == 5500) //再等待0.5秒之后
        {
            Infared_Send_OK(); //红外发射信号让电视继续播放
        }

        if(Run_Static.user_num == 6300)
        {
            Run_Static.step = 0; //步数清零（一次识别已经完成，重新开始）
            Run_Static.user_num = 0;
            Run_Static.point_num = 0;
        }
    }
}


