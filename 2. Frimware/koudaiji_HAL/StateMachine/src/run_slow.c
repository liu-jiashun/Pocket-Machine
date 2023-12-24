#include "run_slow.h"
#include "run_mode.h"
#include "lcd.h"
#include "voice.h"
#include "infared.h"
#include "tripod.h"
#include "relay.h"
#include "logic.h"
#include "diwen_lcd.h"
#include <math.h>

Run_Slow_Struct Run_Slow;

/*慢速模式初始化，主要是将函数指针指向各个实现函数，并将一些变量的值清0*/
void Run_Mode_Slow_Init(void)
{
    Run_Mode_Slow.mode_enter = Run_Mode_Slow_Enter;
    Run_Mode_Slow.mode_exit = Run_Mode_Slow_Exit;
    Run_Mode_Slow.mode_run = Run_Mode_Slow_Run;

    Run_Slow.res_num = 0;
    Run_Slow.step = 0;
    Run_Slow.first_flag = true;
    Run_Slow.last_res.x = 0;
    Run_Slow.last_res.y = 0;
    Run_Slow.res_sum = 0;
    Run_Slow.need_point_num = 0;

    Run_Slow.user_num1 = 0;
    Run_Slow.user_num2 = 0;
    Run_Slow.i = 0;
    Run_Slow.new_flag = false;
}

/*慢速模式进入函数*/
void Run_Mode_Slow_Enter(void)
{
    Run_Slow.res_num = 0;
    Run_Slow.step = 0;
    Run_Slow.first_flag = true;
    Run_Slow.last_res.x = 0;
    Run_Slow.last_res.y = 0;
    Run_Slow.need_point_num = 0;
    Run_Slow.user_num1 = 0;
    Run_Slow.user_num2 = 0;
    Run_Slow.i = 0;
    Run_Slow.new_flag = false;

    Infared.Infared_Is_Pause = false;

    DiWenLcd_Show_RunState(129, "慢速模式"); //迪文屏显示运行模式

    /*因为切换检测物体的形状和颜色的时候也会调用一次模式进入函数，此时不需要播放进入语音*/
    if(enter_voice_flag == true)  //如果允许播放语音
    {
        Voice_Play((uint8_t)SLOW_START);  //播放进入慢速模式的语音
    }
    else  //如果此次不允许播放语音
    {
        enter_voice_flag = true;  //清空该标志位
    }
}

/*慢速模式退出函数*/
void Run_Mode_Slow_Exit(void)
{
    Relay_Control(RELAY_OFF); //关闭激光笔
}

#ifdef SLOW_RIGHT_TO_LEFT

/*慢速模式运行函数*/
void Run_Mode_Slow_Run(void)
{
    /*判断是否是首次检测（首次检测时需要指示屏幕上所有检测到的物体，以后则只需要指示新出现的物体就可以）*/
    if(Run_Slow.first_flag == true)  //首次检测到该物体
    {
        if(Run_Slow.step == 0) //第一步，等待检测到物体
        {
            if(Vision_Dat_Explan() != 0) //如果AI模组检测到物体
            {
                Infared_Send_OK(); //暂停电视机
                Run_Slow.step = 1; //进行下一步
            }
        }
        if(Run_Slow.step == 1) //第二步（暂停之后判断识别的物体是否还稳定存在）
        {
            if(Vision_Dat_Explan() != 0) //如果检测到物体
            {
                Run_Slow.user_num1++;
                if(Run_Slow.user_num1 >= 20) //连续20次检测到物体才认为物体有效（电视接收到红外信号之后有反应时间，可能此时物体已经走到了电视机边缘，造成识别不稳定或者识别不到）
                {
                    Run_Slow.user_num1 = 0;
                    Run_Slow.user_num2 = 0;
                    Run_Slow.step = 2; //进行下一步
                }
            }
            Run_Slow.user_num2++;
            if(Run_Slow.user_num2 >= 3000) //暂停之后3秒内没有检测到物体
            {
                Run_Slow.user_num1 = 0;
                Run_Slow.user_num2 = 0;
                Run_Slow.step = 0; //回到第一步
                Infared_Send_OK(); //电视机继续播放
            }
        }
        if(Run_Slow.step == 2) //第三步
        {
            for(Run_Slow.i=0; Run_Slow.i<Vision_Coord.Res_Num; Run_Slow.i++)
            {
                Run_Slow.res_coord[Run_Slow.i].x = Vision_Coord.Res_Coord[Run_Slow.i].x;
                Run_Slow.res_coord[Run_Slow.i].y = Vision_Coord.Res_Coord[Run_Slow.i].y;  //取出识别到的所有的物体坐标
            }
            Run_Slow.i = 0; //i变量使用完之后清零
            Run_Slow.res_num = Vision_Coord.Res_Num; //赋值当前物品数目变量
            Run_Slow.step = 3;       
        }

        if(Run_Slow.step == 3) //第四步，挨个指向物体
        {
            if((Run_Slow.user_num1 % 5000 == 0) && (Run_Slow.i < Run_Slow.res_num)) //5秒一个，挨个指向物体
            {
                Relay_Control(RELAY_OFF); //指示之前关闭激光笔
                Tripod_X_Coord(Run_Slow.res_coord[Run_Slow.i].x);
                Tripod_Y_Coord(Run_Slow.res_coord[Run_Slow.i].y); //云台运动到指定坐标
                Voice_Play((select_shape-1)*4 + select_color); //语音播报检测到物体
                while(Tripod_Y.is_running);
                while(Tripod_X.is_running); //等待云台运动结束
                Relay_Control(RELAY_ON); //运动结束之后打开激光笔
                //屏幕显示识别到的物体的信息
                Lcd_printf("Detected %s %s:x=%d,y=%d", ColorStr[select_color-1], ShapeStr[select_shape-1],  Run_Slow.res_coord[Run_Slow.i].x, Run_Slow.res_coord[Run_Slow.i].y);
                DiWenLcd_Printf("检测到%s%s: X=%d,Y=%d", ColorStrZh[select_color-1], ShapeStrZh[select_shape-1], Run_Slow.res_coord[Run_Slow.i].x, Run_Slow.res_coord[Run_Slow.i].y);
                Run_Slow.res_sum++; //慢速模式下识别到物体总数的计数+1
				DiWenLcd_Cmd_Data(0x6004, Run_Slow.res_sum); //迪文屏显示识别到物体的总数
                Run_Slow.i++;
                Run_Slow.user_num1 = 0;
            }

            Run_Slow.user_num1++;

            if(Run_Slow.user_num1 == 5001) //程序运行到此处说明上一个判断(i < Run_Slow.res_num)条件不满足，即所有物体指示完毕
            {
                Relay_Control(RELAY_OFF); //首先关闭激光笔（因为激光笔会干扰AI模组的识别）
            }

            if(Run_Slow.user_num1 == 5500) //等待0.5秒之后
            {
                Run_Slow.last_res.x = Run_Slow.res_coord[Run_Slow.res_num-1].x; //保存当前最后一个物体的x坐标
                Run_Slow.last_res.y = Run_Slow.res_coord[Run_Slow.res_num-1].y;
                Infared_Send_OK(); //发射红外信号，电视机继续播放
                Run_Slow.step = 0; //此次识别结束，回到初始状态
                Run_Slow.user_num1 = 0;
                Run_Slow.i = 0;
                Run_Slow.first_flag = false; //清除首次进入标记
            }
        }
    }
    /*不是首次检测该物体时*/
    else
    {
        if(Run_Slow.step == 0)  //第一步
        {
            if(Vision_Dat_Explan() != 0) //如果检测到有物体
            {
                if(Run_Slow.new_flag == false) //如果还没有判断出当前是否有新出现的图形
                {
                    if(Vision_Coord.Res_Coord[Vision_Coord.Res_Num-1].x > (Run_Slow.last_res.x+5)) //如果当前画面中最后一个物体的坐标在上次记录的最后一个物体的坐标右侧
                    {
                        Run_Slow.new_flag = true; //认为当前画面中有新出现的物体
                    }
                    else //如果不满足上述新物体出现的条件，则更新最后一个物体的坐标（即一直跟踪最后一个物体）
                    {
                        Run_Slow.last_res.x = Vision_Coord.Res_Coord[Vision_Coord.Res_Num-1].x;
                        Run_Slow.last_res.y = Vision_Coord.Res_Coord[Vision_Coord.Res_Num-1].y;
                    }
                }
                else  //已经判断出当前画面中有新物体出现
                {
                    for(Run_Slow.i=0; Run_Slow.i<Vision_Coord.Res_Num; Run_Slow.i++)  //遍历当前画面中物体的坐标
                    {
                        if(Vision_Coord.Res_Coord[Run_Slow.i].x >= (Run_Slow.last_res.x + 5)) //找到第一个在上一幅画面中最后一个物体右侧的物体
                        {
                            if(Run_Slow.i > 0) //如果该物体不是这副画面中的第一个物体
                            {
                               Run_Slow.last_res.x = Vision_Coord.Res_Coord[Run_Slow.i-1].x; 
                               Run_Slow.last_res.y = Vision_Coord.Res_Coord[Run_Slow.i-1].y; //重新更新一下跟踪的最后一个物体的坐标
                            }
                            else //如果该物体就是画面中的第一个物体（证明上述一直跟踪的最后一个物体已经流出了当前画面）
                            {
                                Run_Slow.last_res.x = 0; //将跟踪的最后一个物体的坐标直接置0
                                Run_Slow.last_res.y = 0;
                            }
                            break; //找到第一个在上一幅画面中最后一个物体右侧的物体之后直接跳出for循环就行了，不需要再遍历后面的了
                        }
                    }
                }
            }
            if(Run_Slow.new_flag == true)  //如果判断出现了新的物体
            {
                Run_Slow.user_num1 ++;
                if(Run_Slow.user_num1 >= 800) //延时800ms之后再暂停电视机（避免让物体暂停在电视机右侧边缘（最右侧边缘可能出现识别不稳定））
                {
                    Run_Slow.user_num1 = 0; //清空时间控制变量的值
                    Run_Slow.new_flag = false; //清空出现新物体的标志位
                    Infared_Send_OK(); //发送红外信号暂停电视
                    Run_Slow.step = 1; //标记步数到第二步
                }
            }
        }

        if(Run_Slow.step == 1)  //第二步
        {
            if(Vision_Dat_Explan() != 0)  //如果AI模组能够检测到物体
            {
                Run_Slow.user_num1++;
                //同样先遍历物体，找到第一个新出现的物体（因为从第一步发现新出现物体到暂停有800ms，这个时间内可能会新出现更多物体）
                for(Run_Slow.i=0; Run_Slow.i<Vision_Coord.Res_Num; Run_Slow.i++)  
                {
                    if(Vision_Coord.Res_Coord[Run_Slow.i].x > (Run_Slow.last_res.x + 5)) //找到第一个新出现的物体
                    {
                        if(Run_Slow.i > 0) //如果这个物体不是这一帧画面中第一个，重新更新跟踪的最后一个物体的坐标
                        {
                            Run_Slow.last_res.x = Vision_Coord.Res_Coord[Run_Slow.i-1].x; 
                            Run_Slow.last_res.y = Vision_Coord.Res_Coord[Run_Slow.i-1].y; 
                        }
                        else //如果这个物体是这一帧画面中的第一个，将跟踪的最后一个物体的坐标置为0
                        {
                            Run_Slow.last_res.x = 0;
                            Run_Slow.last_res.y = 0;
                        }
                        break; //找到第一个新出现的物体之后直接跳出循环
                    }
                }
                if(Run_Slow.user_num1 >= 30) //连续检测到30帧之后（画面刚暂停时图像不稳定）（识别帧率在30帧左右）
                {
                    for(Run_Slow.i=0; Run_Slow.i<Vision_Coord.Res_Num; Run_Slow.i++) //取出图像的坐标
                    {
                        Run_Slow.res_coord[Run_Slow.i].x = Vision_Coord.Res_Coord[Run_Slow.i].x;
                        Run_Slow.res_coord[Run_Slow.i].y = Vision_Coord.Res_Coord[Run_Slow.i].y;
                    }
                    Run_Slow.res_num = Vision_Coord.Res_Num; //赋值图像数量变量

                    for(Run_Slow.i=0; Run_Slow.i<Run_Slow.res_num; Run_Slow.i++) //重现遍历图像坐标，计算新出现的物品的数量
                    {
                        if(Run_Slow.res_coord[Run_Slow.i].x > Run_Slow.last_res.x) //所有在上一帧最后一个物体右侧的都认为是新的物体
                        {
                            Run_Slow.need_point_num++;
                        }
                    }
                    Run_Slow.i = 0;
                    Run_Slow.step = 2; //步数设置为第三步
                    Run_Slow.user_num1 = 0;
                    Run_Slow.user_num2 = 0; //变量清零
                }
            }
            Run_Slow.user_num2++;
            //上述操作有个连续检测30帧，大概需要1秒就可以运行下一步，所以 Run_Slow.user_num2 变量不会大于3000
            //如果Run_Slow.user_num2 变量大于了3000，则证明上面操作3秒内无法连续稳定检测到图像，则重新播放视频，并将步数从第一步开始
            if(Run_Slow.user_num2 >= 3000)
            {
                Run_Slow.user_num1 = 0;
                Run_Slow.user_num2 = 0; //变量清零
                Infared_Send_OK(); //重新播放视频
                Run_Slow.step = 0; //步数清零
            }
        }
        if(Run_Slow.step == 2) //第三步指向物体
        {
            if((Run_Slow.user_num1 % 5000 == 0) && (Run_Slow.i < Run_Slow.need_point_num)) //5秒一个挨个指示物体
            {
                Relay_Control(RELAY_OFF); //指示之前关闭激光笔
                //屏幕上打印检测到的物体的信息
                Lcd_printf("Detected %s %s:x=%d,y=%d", ColorStr[select_color-1], ShapeStr[select_shape-1], Run_Slow.res_coord[Run_Slow.res_num-1].x, Run_Slow.res_coord[Run_Slow.res_num-1].y);
                DiWenLcd_Printf("检测到%s%s: X=%d,Y=%d", ColorStrZh[select_color-1], ShapeStrZh[select_shape-1], Run_Slow.res_coord[Run_Slow.res_num-1].x, Run_Slow.res_coord[Run_Slow.res_num-1].y);
                Run_Slow.res_sum++; //检测到物体的总数计数+1
				DiWenLcd_Cmd_Data(0x6004, Run_Slow.res_sum); //迪文屏显示总数信息
                Tripod_X_Coord(Run_Slow.res_coord[Run_Slow.res_num-Run_Slow.need_point_num+Run_Slow.i].x);
                Tripod_Y_Coord(Run_Slow.res_coord[Run_Slow.res_num-Run_Slow.need_point_num+Run_Slow.i].y); //云台向物体坐标转动
                Voice_Play((select_shape-1)*4 + select_color); //语音播报检测到物体
                while(Tripod_Y.is_running);
                while(Tripod_X.is_running);  //等待云台转到指定位置
                Relay_Control(RELAY_ON); //打开激光笔
                Run_Slow.i++;
                Run_Slow.user_num1 = 0;
            }

            Run_Slow.user_num1++;

            if(Run_Slow.user_num1 == 5001)
            {
                Relay_Control(RELAY_OFF); //指示结束后关闭激光笔
            }

            if(Run_Slow.user_num1 == 5500) //等待半秒后步数清零，回到初始状态，红外发射信号，电视机继续播放
            {
                Run_Slow.last_res.x = Run_Slow.res_coord[Run_Slow.res_num-1].x; //更新最后一个物体的x坐标
                Run_Slow.last_res.y = Run_Slow.res_coord[Run_Slow.res_num-1].y;
                Infared_Send_OK();

                Run_Slow.user_num1 = 0;
                Run_Slow.step = 0;
                Run_Slow.i = 0;
                Run_Slow.need_point_num = 0;
            }
        }
    }
}

#endif
//如果是视频从上向下播放，逻辑和上述一样，只是新物体的判断改成对比Y坐标。
#ifdef SLOW_UP_TO_DOWN

/*慢速模式运行函数*/
void Run_Mode_Slow_Run(void)
{
    /*判断是否是首次检测（首次检测时需要指示屏幕上所有检测到的物体，以后则只需要指示最后一个就可以）*/
    if(Run_Slow.first_flag == true)  //首次检测到该物体
    {
        if(Run_Slow.step == 0) //第一步，等待检测到物体
        {
            if(Vision_Dat_Explan() != 0) //如果AI模组检测到物体
            {
                Infared_Send_OK(); //暂停电视机
                Run_Slow.step = 1; //进行下一步
            }
        }
        if(Run_Slow.step == 1) //第二步
        {
            if(Vision_Dat_Explan() != 0) //如果检测到物体
            {
                Run_Slow.user_num1++;
                if(Run_Slow.user_num1 >= 20) //连续20次检测到物体才认为物体有效（电视接收到红外信号之后有反应时间，可能此时物体已经走到了电视机边缘，造成识别不稳定或者识别不到）
                {
                    Run_Slow.user_num1 = 0;
                    Run_Slow.user_num2 = 0;
                    Run_Slow.step = 2; //进行下一步
                }
            }
            Run_Slow.user_num2++;
            if(Run_Slow.user_num2 >= 3000) //暂停之后3秒内没有检测到物体
            {
                Run_Slow.user_num1 = 0;
                Run_Slow.user_num2 = 0;
                Run_Slow.step = 0; //回到第一步
                Infared_Send_OK(); //电视机继续播放
            }
        }
        if(Run_Slow.step == 2) //第三步
        {
            for(Run_Slow.i=0; Run_Slow.i<Vision_Coord.Res_Num; Run_Slow.i++)
            {
                Run_Slow.res_coord[Run_Slow.i].x = Vision_Coord.Res_Coord[Run_Slow.i].x;
                Run_Slow.res_coord[Run_Slow.i].y = Vision_Coord.Res_Coord[Run_Slow.i].y;  //取出识别到的所有的物体坐标
            }
            Run_Slow.i = 0;
            Run_Slow.res_num = Vision_Coord.Res_Num;
            Run_Slow.step = 3;       
        }

        if(Run_Slow.step == 3) //第四步，挨个指向物体
        {
            if((Run_Slow.user_num1 % 5000 == 0) && (Run_Slow.i < Run_Slow.res_num)) //5秒一个，挨个指向物体
            {
                Relay_Control(RELAY_OFF); //指示之前关闭激光笔
                Tripod_X_Coord(Run_Slow.res_coord[Run_Slow.i].x);
                Tripod_Y_Coord(Run_Slow.res_coord[Run_Slow.i].y); //云台运动到指定坐标
                Voice_Play((select_shape-1)*4 + select_color); //语音播报检测到物体
                while(Tripod_Y.is_running);
                while(Tripod_X.is_running); //等待云台运动结束
                Relay_Control(RELAY_ON); //运动结束之后打开激光笔
                //屏幕显示识别到的物体的信息
                Lcd_printf("Detected %s %s:x=%d,y=%d", ColorStr[select_color-1], ShapeStr[select_shape-1],  Run_Slow.res_coord[Run_Slow.i].x, Run_Slow.res_coord[Run_Slow.i].y);
                DiWenLcd_Printf("检测到%s%s: X=%d,Y=%d", ColorStrZh[select_color-1], ShapeStrZh[select_shape-1], Run_Slow.res_coord[Run_Slow.i].x, Run_Slow.res_coord[Run_Slow.i].y);
                Run_Slow.res_sum++; //慢速模式下识别到物体总数的计数+1
				DiWenLcd_Cmd_Data(0x6004, Run_Slow.res_sum); //迪文屏显示识别到物体的总数
                Run_Slow.i++;
                Run_Slow.user_num1 = 0;
            }

            Run_Slow.user_num1++;

            if(Run_Slow.user_num1 == 5001) //程序运行到此处说明上一个判断(i < Run_Slow.res_num)条件不满足，即所有物体指示完毕
            {
                Relay_Control(RELAY_OFF); //首先关闭激光笔（因为激光笔会干扰AI模组的识别）
            }

            if(Run_Slow.user_num1 == 5500) //等待1秒之后
            {
                Run_Slow.last_res.x = Run_Slow.res_coord[Run_Slow.res_num-1].x; //保存当前最后一个物体的x坐标
                Run_Slow.last_res.y = Run_Slow.res_coord[Run_Slow.res_num-1].y;
                Infared_Send_OK(); //发射红外信号，电视机继续播放
                Run_Slow.step = 0; //此次识别结束，回到初始状态
                Run_Slow.user_num1 = 0;
                Run_Slow.i = 0;
                Run_Slow.first_flag = false; //清除首次进入标记
            }
        }
    }
    /*不是首次检测该物体时*/
    else
    {
        if(Run_Slow.step == 0)
        {
            if(Vision_Dat_Explan() != 0)
            {
                if(Run_Slow.new_flag == false)
                {
                    if(Vision_Coord.Res_Coord[Vision_Coord.Res_Num-1].y > (Run_Slow.last_res.y+5))
                    {
                        Run_Slow.new_flag = true;
                    }
                    else
                    {
                        Run_Slow.last_res.y = Vision_Coord.Res_Coord[Vision_Coord.Res_Num-1].y;
                    }
                }
                else
                {
                    for(Run_Slow.i=0; Run_Slow.i<Vision_Coord.Res_Num; Run_Slow.i++)
                    {
                        if(Vision_Coord.Res_Coord[Run_Slow.i].y >= (Run_Slow.last_res.y + 5))
                        {
                            if(Run_Slow.i > 0)
                            {
                               Run_Slow.last_res.y = Vision_Coord.Res_Coord[Run_Slow.i-1].y; 
                            }
                            else
                            {
                                Run_Slow.last_res.y = 0;
                            }
                            break;
                        }
                    }
                }
            }
            if(Run_Slow.new_flag == true)
            {
                Run_Slow.user_num1 ++;
                if(Run_Slow.user_num1 >= 800)
                {
                    Run_Slow.user_num1 = 0;
                    Run_Slow.new_flag = false;
                    Infared_Send_OK();
                    Run_Slow.step = 1;
                }
            }
        }

        if(Run_Slow.step == 1)
        {
            if(Vision_Dat_Explan() != 0)
            {
                Run_Slow.user_num1++;
                for(Run_Slow.i=0; Run_Slow.i<Vision_Coord.Res_Num; Run_Slow.i++)
                {
                    if(Vision_Coord.Res_Coord[Run_Slow.i].y > (Run_Slow.last_res.y + 5))
                    {
                        if(Run_Slow.i > 0)
                        {
                            Run_Slow.last_res.y = Vision_Coord.Res_Coord[Run_Slow.i-1].y; 
                        }
                        else
                        {
                            Run_Slow.last_res.y = 0;
                        }
                        break;
                    }
                }
                if(Run_Slow.user_num1 >= 30)
                {
                    for(Run_Slow.i=0; Run_Slow.i<Vision_Coord.Res_Num; Run_Slow.i++)
                    {
                        Run_Slow.res_coord[Run_Slow.i].x = Vision_Coord.Res_Coord[Run_Slow.i].x;
                        Run_Slow.res_coord[Run_Slow.i].y = Vision_Coord.Res_Coord[Run_Slow.i].y;
                    }
                    Run_Slow.res_num = Vision_Coord.Res_Num;

                    for(Run_Slow.i=0; Run_Slow.i<Run_Slow.res_num; Run_Slow.i++)
                    {
                        if(Run_Slow.res_coord[Run_Slow.i].y > Run_Slow.last_res.y)
                        {
                            Run_Slow.need_point_num++;
                        }
                    }
                    Run_Slow.i = 0;
                    Run_Slow.step = 2;
                    Run_Slow.user_num1 = 0;
                    Run_Slow.user_num2 = 0;
                }
            }
            Run_Slow.user_num2++;
            if(Run_Slow.user_num2 >= 3000)
            {
                Run_Slow.user_num1 = 0;
                Run_Slow.user_num2 = 0;
                Run_Slow.step = 0;
            }
        }
        if(Run_Slow.step == 2) //第三步指向物体
        {
            if((Run_Slow.user_num1 % 5000 == 0) && (Run_Slow.i < Run_Slow.need_point_num))
            {
                Relay_Control(RELAY_OFF); //指示之前关闭激光笔
                //屏幕上打印检测到的物体的信息
                Lcd_printf("Detected %s %s:x=%d,y=%d", ColorStr[select_color-1], ShapeStr[select_shape-1], Run_Slow.res_coord[Run_Slow.res_num-Run_Slow.need_point_num+Run_Slow.i].x, Run_Slow.res_coord[Run_Slow.res_num-Run_Slow.need_point_num+Run_Slow.i].y);
                DiWenLcd_Printf("检测到%s%s: X=%d,Y=%d", ColorStrZh[select_color-1], ShapeStrZh[select_shape-1], Run_Slow.res_coord[Run_Slow.res_num-Run_Slow.need_point_num+Run_Slow.i].x, Run_Slow.res_coord[Run_Slow.res_num-Run_Slow.need_point_num+Run_Slow.i].y);
                Run_Slow.res_sum++; //检测到物体的总数计数+1
				DiWenLcd_Cmd_Data(0x6004, Run_Slow.res_sum); //迪文屏显示总数信息
                Tripod_X_Coord(Run_Slow.res_coord[Run_Slow.res_num-Run_Slow.need_point_num+Run_Slow.i].x);
                Tripod_Y_Coord(Run_Slow.res_coord[Run_Slow.res_num-Run_Slow.need_point_num+Run_Slow.i].y); //云台向物体坐标转动
                Voice_Play((select_shape-1)*4 + select_color); //语音播报检测到物体
                while(Tripod_Y.is_running);
                while(Tripod_X.is_running);  //等待云台转到指定位置
                Relay_Control(RELAY_ON); //打开激光笔
                Run_Slow.i++;
                Run_Slow.user_num1 = 0;
            }

            Run_Slow.user_num1++;

            if(Run_Slow.user_num1 == 5001)
            {
                Relay_Control(RELAY_OFF); //指示结束后关闭激光笔
            }

            if(Run_Slow.user_num1 == 5500) //等待一秒后步数清零，回到初始状态，红外发射信号，电视机继续播放
            {
                Run_Slow.last_res.x = Run_Slow.res_coord[Run_Slow.res_num-1].x; //更新最后一个物体的x坐标
                Run_Slow.last_res.y = Run_Slow.res_coord[Run_Slow.res_num-1].y;
                Infared_Send_OK();

                Run_Slow.user_num1 = 0;
                Run_Slow.step = 0;
                Run_Slow.i = 0;
                Run_Slow.need_point_num = 0;
            }
        }
    }
}

#endif

#ifdef SLOW_LEFT_TO_RIGHT

/*慢速模式运行函数*/
void Run_Mode_Slow_Run(void)
{
    /*判断是否是首次检测（首次检测时需要指示屏幕上所有检测到的物体，以后则只需要指示新出现的物体就可以）*/
    if(Run_Slow.first_flag == true)  //首次检测到该物体
    {
        if(Run_Slow.step == 0) //第一步，等待检测到物体
        {
            if(Vision_Dat_Explan() != 0) //如果AI模组检测到物体
            {
                Infared_Send_OK(); //暂停电视机
                Run_Slow.step = 1; //进行下一步
            }
        }
        if(Run_Slow.step == 1) //第二步（暂停之后判断识别的物体是否还稳定存在）
        {
            if(Vision_Dat_Explan() != 0) //如果检测到物体
            {
                Run_Slow.user_num1++;
                if(Run_Slow.user_num1 >= 20) //连续20次检测到物体才认为物体有效（电视接收到红外信号之后有反应时间，可能此时物体已经走到了电视机边缘，造成识别不稳定或者识别不到）
                {
                    Run_Slow.user_num1 = 0;
                    Run_Slow.user_num2 = 0;
                    Run_Slow.step = 2; //进行下一步
                }
            }
            Run_Slow.user_num2++;
            if(Run_Slow.user_num2 >= 3000) //暂停之后3秒内没有检测到物体
            {
                Run_Slow.user_num1 = 0;
                Run_Slow.user_num2 = 0;
                Run_Slow.step = 0; //回到第一步
                Infared_Send_OK(); //电视机继续播放
            }
        }
        if(Run_Slow.step == 2) //第三步
        {
            for(Run_Slow.i=0; Run_Slow.i<Vision_Coord.Res_Num; Run_Slow.i++)
            {
                Run_Slow.res_coord[Run_Slow.i].x = Vision_Coord.Res_Coord[Run_Slow.i].x;
                Run_Slow.res_coord[Run_Slow.i].y = Vision_Coord.Res_Coord[Run_Slow.i].y;  //取出识别到的所有的物体坐标
            }
            Run_Slow.i = 0; //i变量使用完之后清零
            Run_Slow.res_num = Vision_Coord.Res_Num; //赋值当前物品数目变量
            Run_Slow.step = 3;       
        }

        if(Run_Slow.step == 3) //第四步，挨个指向物体
        {
            if((Run_Slow.user_num1 % 5000 == 0) && (Run_Slow.i < Run_Slow.res_num)) //5秒一个，挨个指向物体
            {
                Relay_Control(RELAY_OFF); //指示之前关闭激光笔
                Tripod_X_Coord(Run_Slow.res_coord[Run_Slow.i].x);
                Tripod_Y_Coord(Run_Slow.res_coord[Run_Slow.i].y); //云台运动到指定坐标
                Voice_Play((select_shape-1)*4 + select_color); //语音播报检测到物体
                while(Tripod_Y.is_running);
                while(Tripod_X.is_running); //等待云台运动结束
                Relay_Control(RELAY_ON); //运动结束之后打开激光笔
                //屏幕显示识别到的物体的信息
                Lcd_printf("Detected %s %s:x=%d,y=%d", ColorStr[select_color-1], ShapeStr[select_shape-1],  Run_Slow.res_coord[Run_Slow.i].x, Run_Slow.res_coord[Run_Slow.i].y);
                DiWenLcd_Printf("检测到%s%s: X=%d,Y=%d", ColorStrZh[select_color-1], ShapeStrZh[select_shape-1], Run_Slow.res_coord[Run_Slow.i].x, Run_Slow.res_coord[Run_Slow.i].y);
                Run_Slow.res_sum++; //慢速模式下识别到物体总数的计数+1
				DiWenLcd_Cmd_Data(0x6004, Run_Slow.res_sum); //迪文屏显示识别到物体的总数
                Run_Slow.i++;
                Run_Slow.user_num1 = 0;
            }

            Run_Slow.user_num1++;

            if(Run_Slow.user_num1 == 5001) //程序运行到此处说明上一个判断(i < Run_Slow.res_num)条件不满足，即所有物体指示完毕
            {
                Relay_Control(RELAY_OFF); //首先关闭激光笔（因为激光笔会干扰AI模组的识别）
            }

            if(Run_Slow.user_num1 == 5500) //等待0.5秒之后
            {
                Run_Slow.last_res.x = Run_Slow.res_coord[0].x; //保存当前最后一个物体的x坐标
                Run_Slow.last_res.y = Run_Slow.res_coord[0].y;
                Infared_Send_OK(); //发射红外信号，电视机继续播放
                Run_Slow.step = 0; //此次识别结束，回到初始状态
                Run_Slow.user_num1 = 0;
                Run_Slow.i = 0;
                Run_Slow.first_flag = false; //清除首次进入标记
            }
        }
    }
    /*不是首次检测该物体时*/
    else
    {
        if(Run_Slow.step == 0)  //第一步
        {
            if(Vision_Dat_Explan() != 0) //如果检测到有物体
            {
                if(Run_Slow.new_flag == false) //如果还没有判断出当前是否有新出现的图形
                {
                    if(Vision_Coord.Res_Coord[0].x < (Run_Slow.last_res.x-5)) //如果当前画面中最后一个物体的坐标在上次记录的最后一个物体的坐标右侧
                    {
                        Run_Slow.new_flag = true; //认为当前画面中有新出现的物体
                    }
                    else //如果不满足上述新物体出现的条件，则更新最后一个物体的坐标（即一直跟踪最后一个物体）
                    {
                        Run_Slow.last_res.x = Vision_Coord.Res_Coord[0].x;
                        Run_Slow.last_res.y = Vision_Coord.Res_Coord[0].y;
                    }
                }
                else  //已经判断出当前画面中有新物体出现
                {
                    for(Run_Slow.i=(Vision_Coord.Res_Num-1); Run_Slow.i>=0; Run_Slow.i--)  //遍历当前画面中物体的坐标
                    {
                        if(Vision_Coord.Res_Coord[Run_Slow.i].x <= (Run_Slow.last_res.x - 5)) //找到第一个在上一幅画面中最后一个物体右侧的物体
                        {
                            if(Run_Slow.i < (Vision_Coord.Res_Num-1)) //如果该物体不是这副画面中的第一个物体
                            {
                               Run_Slow.last_res.x = Vision_Coord.Res_Coord[Run_Slow.i+1].x; 
                               Run_Slow.last_res.y = Vision_Coord.Res_Coord[Run_Slow.i+1].y; //重新更新一下跟踪的最后一个物体的坐标
                            }
                            else //如果该物体就是画面中的第一个物体（证明上述一直跟踪的最后一个物体已经流出了当前画面）
                            {
                                Run_Slow.last_res.x = 1365; //将跟踪的最后一个物体的坐标直接置0
                                Run_Slow.last_res.y = 767;
                            }
                            break; //找到第一个在上一幅画面中最后一个物体右侧的物体之后直接跳出for循环就行了，不需要再遍历后面的了
                        }
                    }
                }
            }
            if(Run_Slow.new_flag == true)  //如果判断出现了新的物体
            {
                Run_Slow.user_num1 ++;
                if(Run_Slow.user_num1 >= 800) //延时800ms之后再暂停电视机（避免让物体暂停在电视机右侧边缘（最右侧边缘可能出现识别不稳定））
                {
                    Run_Slow.user_num1 = 0; //清空时间控制变量的值
                    Run_Slow.new_flag = false; //清空出现新物体的标志位
                    Infared_Send_OK(); //发送红外信号暂停电视
                    Run_Slow.step = 1; //标记步数到第二步
                }
            }
        }

        if(Run_Slow.step == 1)  //第二步
        {
            if(Vision_Dat_Explan() != 0)  //如果AI模组能够检测到物体
            {
                Run_Slow.user_num1++;
                //同样先遍历物体，找到第一个新出现的物体（因为从第一步发现新出现物体到暂停有800ms，这个时间内可能会新出现更多物体）
                for(Run_Slow.i=(Vision_Coord.Res_Num-1); Run_Slow.i>=0; Run_Slow.i--)  
                {
                    if(Vision_Coord.Res_Coord[Run_Slow.i].x < (Run_Slow.last_res.x - 5)) //找到第一个新出现的物体
                    {
                        if(Run_Slow.i < (Vision_Coord.Res_Num-1)) //如果这个物体不是这一帧画面中第一个，重新更新跟踪的最后一个物体的坐标
                        {
                            Run_Slow.last_res.x = Vision_Coord.Res_Coord[Run_Slow.i+1].x; 
                            Run_Slow.last_res.y = Vision_Coord.Res_Coord[Run_Slow.i+1].y; 
                        }
                        else //如果这个物体是这一帧画面中的第一个，将跟踪的最后一个物体的坐标置为0
                        {
                            Run_Slow.last_res.x = 1365;
                            Run_Slow.last_res.y = 767;
                        }
                        break; //找到第一个新出现的物体之后直接跳出循环
                    }
                }
                if(Run_Slow.user_num1 >= 30) //连续检测到30帧之后（画面刚暂停时图像不稳定）（识别帧率在30帧左右）
                {
                    for(Run_Slow.i=0; Run_Slow.i<Vision_Coord.Res_Num; Run_Slow.i++) //取出图像的坐标
                    {
                        Run_Slow.res_coord[Run_Slow.i].x = Vision_Coord.Res_Coord[Run_Slow.i].x;
                        Run_Slow.res_coord[Run_Slow.i].y = Vision_Coord.Res_Coord[Run_Slow.i].y;
                    }
                    Run_Slow.res_num = Vision_Coord.Res_Num; //赋值图像数量变量

                    for(Run_Slow.i=0; Run_Slow.i<Run_Slow.res_num; Run_Slow.i++) //重现遍历图像坐标，计算新出现的物品的数量
                    {
                        if(Run_Slow.res_coord[Run_Slow.i].x < Run_Slow.last_res.x) //所有在上一帧最后一个物体右侧的都认为是新的物体
                        {
                            Run_Slow.need_point_num++;
                        }
                    }
                    Run_Slow.i = 0;
                    Run_Slow.step = 2; //步数设置为第三步
                    Run_Slow.user_num1 = 0;
                    Run_Slow.user_num2 = 0; //变量清零
                }
            }
            Run_Slow.user_num2++;
            //上述操作有个连续检测30帧，大概需要1秒就可以运行下一步，所以 Run_Slow.user_num2 变量不会大于3000
            //如果Run_Slow.user_num2 变量大于了3000，则证明上面操作3秒内无法连续稳定检测到图像，则重新播放视频，并将步数从第一步开始
            if(Run_Slow.user_num2 >= 3000)
            {
                Run_Slow.user_num1 = 0;
                Run_Slow.user_num2 = 0; //变量清零
                Infared_Send_OK(); //重新播放视频
                Run_Slow.step = 0; //步数清零
            }
        }
        if(Run_Slow.step == 2) //第三步指向物体
        {
            if((Run_Slow.user_num1 % 5000 == 0) && (Run_Slow.i < Run_Slow.need_point_num)) //5秒一个挨个指示物体
            {
                Relay_Control(RELAY_OFF); //指示之前关闭激光笔
                //屏幕上打印检测到的物体的信息
                Lcd_printf("Detected %s %s:x=%d,y=%d", ColorStr[select_color-1], ShapeStr[select_shape-1], Run_Slow.res_coord[Run_Slow.i].x, Run_Slow.res_coord[Run_Slow.i].y);
                DiWenLcd_Printf("检测到%s%s: X=%d,Y=%d", ColorStrZh[select_color-1], ShapeStrZh[select_shape-1], Run_Slow.res_coord[Run_Slow.i].x, Run_Slow.res_coord[Run_Slow.i].y);
                Run_Slow.res_sum++; //检测到物体的总数计数+1
				DiWenLcd_Cmd_Data(0x6004, Run_Slow.res_sum); //迪文屏显示总数信息
                Tripod_X_Coord(Run_Slow.res_coord[Run_Slow.i].x);
                Tripod_Y_Coord(Run_Slow.res_coord[Run_Slow.i].y); //云台向物体坐标转动
                Voice_Play((select_shape-1)*4 + select_color); //语音播报检测到物体
                while(Tripod_Y.is_running);
                while(Tripod_X.is_running);  //等待云台转到指定位置
                Relay_Control(RELAY_ON); //打开激光笔
                Run_Slow.i++;
                Run_Slow.user_num1 = 0;
            }

            Run_Slow.user_num1++;

            if(Run_Slow.user_num1 == 5001)
            {
                Relay_Control(RELAY_OFF); //指示结束后关闭激光笔
            }

            if(Run_Slow.user_num1 == 5500) //等待半秒后步数清零，回到初始状态，红外发射信号，电视机继续播放
            {
                Run_Slow.last_res.x = Run_Slow.res_coord[0].x; //更新最后一个物体的x坐标
                Run_Slow.last_res.y = Run_Slow.res_coord[0].y;
                Infared_Send_OK();

                Run_Slow.user_num1 = 0;
                Run_Slow.step = 0;
                Run_Slow.i = 0;
                Run_Slow.need_point_num = 0;
            }
        }
    }
}

#endif



