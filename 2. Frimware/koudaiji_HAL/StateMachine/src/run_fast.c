/*
*����ģʽʵ���߼���������ģʽ��ͬ
*/
#include "run_fast.h"
#include "run_mode.h"
#include "lcd.h"
#include "voice.h"
#include "infared.h"
#include "tripod.h"
#include "relay.h"
#include "diwen_lcd.h"
#include <math.h>

Run_Fast_Struct Run_Fast; //�������ģʽ�ṹ�����

/*����ģʽ��ʼ��*/
void Run_Mode_Fast_Init(void)
{
    Run_Mode_Fast.mode_enter = Run_Mode_Fast_Enter;
    Run_Mode_Fast.mode_exit = Run_Mode_Fast_Exit;
    Run_Mode_Fast.mode_run = Run_Mode_Fast_Run;  //������ģʽ���롢�˳�����������������ʵ��ע�ᵽ����ָ��

    Run_Fast.res_num = 0;
    Run_Fast.step = 0;
    Run_Fast.first_flag = true;
    Run_Fast.last_res.x = 0;
    Run_Fast.last_res.y = 0;
    Run_Fast.res_sum = 0;  //�����ر���
    Run_Fast.need_point_num = 0;

    Run_Fast.user_num1 = 0;
    Run_Fast.user_num2 = 0;
    Run_Fast.i = 0;
    Run_Fast.new_flag = false;
}

/*����ģʽ����*/
void Run_Mode_Fast_Enter(void)
{
    Run_Fast.res_num = 0;
    Run_Fast.step = 0;
    Run_Fast.first_flag = true;
    Run_Fast.last_res.x = 0;
    Run_Fast.last_res.y = 0;  //�����ر���
    Run_Fast.need_point_num = 0;
    Run_Fast.user_num1 = 0;
    Run_Fast.user_num2 = 0;
    Run_Fast.i = 0;
    Run_Fast.new_flag = false;

    Infared.Infared_Is_Pause = false;

    DiWenLcd_Show_RunState(129, "����ģʽ");  //��������ʾ��ص�״̬��Ϣ

    if(enter_voice_flag == true) //�������������
    {
        Voice_Play((uint8_t)FAST_START); //���Ž������ģʽ������
    }
    else //����˴β�����������
    {
        enter_voice_flag = true; //��ոñ�־λ
    }
}

/*����ģʽ�˳�*/
void Run_Mode_Fast_Exit(void)
{
    Relay_Control(RELAY_OFF); //�رռ����
}

#ifdef FAST_RIGHT_TO_LEFT
/*����ģʽ���к���*/
void Run_Mode_Fast_Run(void)
{
    if(Run_Fast.first_flag == true) //�ж��Ƿ����״μ�⣨�״μ��ʱ��Ҫָʾ��Ļ�����м�⵽�����壬�Ժ���ֻ��Ҫָʾ���һ���Ϳ��ԣ�
    {
        if(Run_Fast.step == 0)
        {
            if(Vision_Dat_Explan() != 0)
            {
                Run_Fast.user_num1++;
                if(Run_Fast.user_num1 >= 5)
                {
                    Infared_Send_OK();
                    Run_Fast.step = 1; 
                    Run_Fast.user_num1 = 0;
                }
            }
        }
        if(Run_Fast.step == 1)
        {
            if(Vision_Dat_Explan() != 0)
            {
                Run_Fast.user_num1++;
                if(Run_Fast.user_num1 >= 20)
                {
                    Run_Fast.user_num1 = 0;
                    Run_Fast.user_num2 = 0;
                    Run_Fast.step = 2;
                }
            }
            Run_Fast.user_num2++;
            if(Run_Fast.user_num2 >= 3000)
            {
                Run_Fast.user_num2 = 0;
                Run_Fast.user_num1 = 0;
                Run_Fast.step = 0;
                Infared_Send_OK();
            }
        }
        if(Run_Fast.step == 2)
        {
            for(Run_Fast.i=0; Run_Fast.i<Vision_Coord.Res_Num; Run_Fast.i++)
            {
                Run_Fast.res_coord[Run_Fast.i].x = Vision_Coord.Res_Coord[Run_Fast.i].x;
                Run_Fast.res_coord[Run_Fast.i].y = Vision_Coord.Res_Coord[Run_Fast.i].y;
            }
            Run_Fast.i = 0;
            Run_Fast.res_num = Vision_Coord.Res_Num;
            Run_Fast.step = 3;       
        }

        if(Run_Fast.step == 3)
        {
            if((Run_Fast.user_num1 % 5000 == 0) && (Run_Fast.i < Run_Fast.res_num))
            {
                Relay_Control(RELAY_OFF);
                Tripod_X_Coord(Run_Fast.res_coord[Run_Fast.i].x);
                Tripod_Y_Coord(Run_Fast.res_coord[Run_Fast.i].y);
                while(Tripod_Y.is_running);
                while(Tripod_X.is_running);
                Voice_Play((select_shape-1)*4 + select_color);
                Relay_Control(RELAY_ON);
                Lcd_printf("Detected %s %s:x=%d,y=%d", ColorStr[select_color-1], ShapeStr[select_shape-1],  Run_Fast.res_coord[Run_Fast.i].x, Run_Fast.res_coord[Run_Fast.i].y);
                DiWenLcd_Printf("��⵽%s%s: X=%d,Y=%d", ColorStrZh[select_color-1], ShapeStrZh[select_shape-1], Run_Fast.res_coord[Run_Fast.i].x, Run_Fast.res_coord[Run_Fast.i].y);
                Run_Fast.res_sum++;
				DiWenLcd_Cmd_Data(0x6006, Run_Fast.res_sum);
                Run_Fast.i++;
                Run_Fast.user_num1 = 0;
            }

            Run_Fast.user_num1++;

            if(Run_Fast.user_num1 == 5001)
            {
                Relay_Control(RELAY_OFF);
            }

            if(Run_Fast.user_num1 == 5500)
            {
                Run_Fast.last_res.x = Run_Fast.res_coord[Run_Fast.res_num-1].x;
                Run_Fast.last_res.y = Run_Fast.res_coord[Run_Fast.res_num-1].y;
                Infared_Send_OK();
            }

            if(Run_Fast.user_num1 == 6000)
            {
                Run_Fast.step = 0;
                Run_Fast.user_num1 = 0;
                Run_Fast.i = 0;
                Run_Fast.first_flag = false;
            }
        }
    }
    /*���ǵ�һ�ν���ʱ*/
    else
    {
        if(Run_Fast.step == 0)
        {
            if(Vision_Dat_Explan() != 0)
            {
                if(Run_Fast.new_flag == false)
                {
                    if(Vision_Coord.Res_Coord[Vision_Coord.Res_Num-1].x > (Run_Fast.last_res.x+5))
                    {
                        Run_Fast.new_flag = true;
                    }
                    else
                    {
                        Run_Fast.last_res.x = Vision_Coord.Res_Coord[Vision_Coord.Res_Num-1].x;
                        Run_Fast.last_res.y = Vision_Coord.Res_Coord[Vision_Coord.Res_Num-1].y;
                    }
                }
                else
                {
                    for(Run_Fast.i=0; Run_Fast.i<Vision_Coord.Res_Num; Run_Fast.i++)
                    {
                        if(Vision_Coord.Res_Coord[Run_Fast.i].x >= (Run_Fast.last_res.x + 5))
                        {
                            if(Run_Fast.i > 0)
                            {
                               Run_Fast.last_res.x = Vision_Coord.Res_Coord[Run_Fast.i-1].x; 
                               Run_Fast.last_res.y = Vision_Coord.Res_Coord[Run_Fast.i-1].y;
                            }
                            else
                            {
                                Run_Fast.last_res.x = 0;
                                Run_Fast.last_res.y = 0;
                            }
                            break;
                        }
                    }
                }
            }
            if(Run_Fast.new_flag == true)
            {
                Run_Fast.user_num1 ++;
                if(Run_Fast.user_num1 >= 1000)
                {
                    Run_Fast.user_num1 = 0;
                    Run_Fast.new_flag = false;
                    Infared_Send_OK();
                    Run_Fast.step = 1;
                }
            }
        }
        if(Run_Fast.step == 1)
        {
            if(Vision_Dat_Explan() != 0)
            {
                Run_Fast.user_num1++;
                for(Run_Fast.i=0; Run_Fast.i<Vision_Coord.Res_Num; Run_Fast.i++)
                {
                    if(Vision_Coord.Res_Coord[Run_Fast.i].x > (Run_Fast.last_res.x + 5))
                    {
                        if(Run_Fast.i > 0)
                        {
                            Run_Fast.last_res.x = Vision_Coord.Res_Coord[Run_Fast.i-1].x; 
                            Run_Fast.last_res.y = Vision_Coord.Res_Coord[Run_Fast.i-1].y; 
                        }
                        else
                        {
                            Run_Fast.last_res.x = 0;
                            Run_Fast.last_res.y = 0;
                        }
                        break;
                    }
                }
                if(Run_Fast.user_num1 >= 30)
                {
                    for(Run_Fast.i=0; Run_Fast.i<Vision_Coord.Res_Num; Run_Fast.i++)
                    {
                        Run_Fast.res_coord[Run_Fast.i].x = Vision_Coord.Res_Coord[Run_Fast.i].x;
                        Run_Fast.res_coord[Run_Fast.i].y = Vision_Coord.Res_Coord[Run_Fast.i].y;
                    }
                    Run_Fast.res_num = Vision_Coord.Res_Num;

                    for(Run_Fast.i=0; Run_Fast.i<Run_Fast.res_num; Run_Fast.i++)
                    {
                        if(Run_Fast.res_coord[Run_Fast.i].x > Run_Fast.last_res.x)
                        {
                            Run_Fast.need_point_num++;
                        }
                    }
                    Run_Fast.i = 0;
                    Run_Fast.step = 2;
                    Run_Fast.user_num1 = 0;
                    Run_Fast.user_num2 = 0;
                }
            }
            Run_Fast.user_num2++;
            if(Run_Fast.user_num2 >= 3000)
            {
                Run_Fast.user_num1 = 0;
                Run_Fast.user_num2 = 0;
                Infared_Send_OK();
                Run_Fast.step = 0;
            }
        }
        if(Run_Fast.step == 2)
        {
            if((Run_Fast.user_num1 % 5000 == 0) && (Run_Fast.i < Run_Fast.need_point_num))
            {
                Relay_Control(RELAY_OFF);
                Lcd_printf("Detected %s %s:x=%d,y=%d", ColorStr[select_color-1], ShapeStr[select_shape-1], Run_Fast.res_coord[Run_Fast.res_num-Run_Fast.need_point_num+Run_Fast.i].x, Run_Fast.res_coord[Run_Fast.res_num-Run_Fast.need_point_num+Run_Fast.i].y);
                DiWenLcd_Printf("��⵽%s%s: X=%d,Y=%d", ColorStrZh[select_color-1], ShapeStrZh[select_shape-1], Run_Fast.res_coord[Run_Fast.res_num-Run_Fast.need_point_num+Run_Fast.i].x, Run_Fast.res_coord[Run_Fast.res_num-Run_Fast.need_point_num+Run_Fast.i].y);
                Run_Fast.res_sum++;
				DiWenLcd_Cmd_Data(0x6006, Run_Fast.res_sum);
                Tripod_X_Coord(Run_Fast.res_coord[Run_Fast.res_num-Run_Fast.need_point_num+Run_Fast.i].x);
                Tripod_Y_Coord(Run_Fast.res_coord[Run_Fast.res_num-Run_Fast.need_point_num+Run_Fast.i].y);
                Voice_Play((select_shape-1)*4 + select_color);
                while(Tripod_Y.is_running);
                while(Tripod_X.is_running);
                Relay_Control(RELAY_ON);
                Run_Fast.i++;
                Run_Fast.user_num1 = 0;
            }

            Run_Fast.user_num1++;

            if(Run_Fast.user_num1 == 5001)
            {
                Relay_Control(RELAY_OFF);
            }

            if(Run_Fast.user_num1 == 5500)
            {
                Run_Fast.last_res.x = Run_Fast.res_coord[Run_Fast.res_num-1].x;
                Run_Fast.last_res.y = Run_Fast.res_coord[Run_Fast.res_num-1].y;
                Infared_Send_OK();
            }

            if(Run_Fast.user_num1 == 6000)
            {
                Run_Fast.user_num1 = 0;
                Run_Fast.step = 0;
                Run_Fast.i = 0;
                Run_Fast.need_point_num = 0;
            }
        }
    }
}

#endif

#ifdef FAST_LEFT_TO_RIGHT

/*����ģʽ���к���*/
void Run_Mode_Fast_Run(void)
{
    /*�ж��Ƿ����״μ�⣨�״μ��ʱ��Ҫָʾ��Ļ�����м�⵽�����壬�Ժ���ֻ��Ҫָʾ�³��ֵ�����Ϳ��ԣ�*/
    if(Run_Fast.first_flag == true)  //�״μ�⵽������
    {
        if(Run_Fast.step == 0) //��һ�����ȴ���⵽����
        {
            if(Vision_Dat_Explan() != 0) //���AIģ���⵽����
            {
                Infared_Send_OK(); //��ͣ���ӻ�
                Run_Fast.step = 1; //������һ��
            }
        }
        if(Run_Fast.step == 1) //�ڶ�������֮ͣ���ж�ʶ��������Ƿ��ȶ����ڣ�
        {
            if(Vision_Dat_Explan() != 0) //�����⵽����
            {
                Run_Fast.user_num1++;
                if(Run_Fast.user_num1 >= 20) //����20�μ�⵽�������Ϊ������Ч�����ӽ��յ������ź�֮���з�Ӧʱ�䣬���ܴ�ʱ�����Ѿ��ߵ��˵��ӻ���Ե�����ʶ���ȶ�����ʶ�𲻵���
                {
                    Run_Fast.user_num1 = 0;
                    Run_Fast.user_num2 = 0;
                    Run_Fast.step = 2; //������һ��
                }
            }
            Run_Fast.user_num2++;
            if(Run_Fast.user_num2 >= 3000) //��֮ͣ��3����û�м�⵽����
            {
                Run_Fast.user_num1 = 0;
                Run_Fast.user_num2 = 0;
                Run_Fast.step = 0; //�ص���һ��
                Infared_Send_OK(); //���ӻ���������
            }
        }
        if(Run_Fast.step == 2) //������
        {
            for(Run_Fast.i=0; Run_Fast.i<Vision_Coord.Res_Num; Run_Fast.i++)
            {
                Run_Fast.res_coord[Run_Fast.i].x = Vision_Coord.Res_Coord[Run_Fast.i].x;
                Run_Fast.res_coord[Run_Fast.i].y = Vision_Coord.Res_Coord[Run_Fast.i].y;  //ȡ��ʶ�𵽵����е���������
            }
            Run_Fast.i = 0; //i����ʹ����֮������
            Run_Fast.res_num = Vision_Coord.Res_Num; //��ֵ��ǰ��Ʒ��Ŀ����
            Run_Fast.step = 3;       
        }

        if(Run_Fast.step == 3) //���Ĳ�������ָ������
        {
            if((Run_Fast.user_num1 % 5000 == 0) && (Run_Fast.i < Run_Fast.res_num)) //5��һ��������ָ������
            {
                Relay_Control(RELAY_OFF); //ָʾ֮ǰ�رռ����
                Tripod_X_Coord(Run_Fast.res_coord[Run_Fast.i].x);
                Tripod_Y_Coord(Run_Fast.res_coord[Run_Fast.i].y); //��̨�˶���ָ������
                Voice_Play((select_shape-1)*4 + select_color); //����������⵽����
                while(Tripod_Y.is_running);
                while(Tripod_X.is_running); //�ȴ���̨�˶�����
                Relay_Control(RELAY_ON); //�˶�����֮��򿪼����
                //��Ļ��ʾʶ�𵽵��������Ϣ
                Lcd_printf("Detected %s %s:x=%d,y=%d", ColorStr[select_color-1], ShapeStr[select_shape-1],  Run_Fast.res_coord[Run_Fast.i].x, Run_Fast.res_coord[Run_Fast.i].y);
                DiWenLcd_Printf("��⵽%s%s: X=%d,Y=%d", ColorStrZh[select_color-1], ShapeStrZh[select_shape-1], Run_Fast.res_coord[Run_Fast.i].x, Run_Fast.res_coord[Run_Fast.i].y);
                Run_Fast.res_sum++; //����ģʽ��ʶ�����������ļ���+1
				DiWenLcd_Cmd_Data(0x6004, Run_Fast.res_sum); //��������ʾʶ�����������
                Run_Fast.i++;
                Run_Fast.user_num1 = 0;
            }

            Run_Fast.user_num1++;

            if(Run_Fast.user_num1 == 5001) //�������е��˴�˵����һ���ж�(i < Run_Fast.res_num)���������㣬����������ָʾ���
            {
                Relay_Control(RELAY_OFF); //���ȹرռ���ʣ���Ϊ����ʻ����AIģ���ʶ��
            }

            if(Run_Fast.user_num1 == 5500) //�ȴ�0.5��֮��
            {
                Run_Fast.last_res.x = Run_Fast.res_coord[0].x; //���浱ǰ���һ�������x����
                Run_Fast.last_res.y = Run_Fast.res_coord[0].y;
                Infared_Send_OK(); //��������źţ����ӻ���������
                Run_Fast.step = 0; //�˴�ʶ��������ص���ʼ״̬
                Run_Fast.user_num1 = 0;
                Run_Fast.i = 0;
                Run_Fast.first_flag = false; //����״ν�����
            }
        }
    }
    /*�����״μ�������ʱ*/
    else
    {
        if(Run_Fast.step == 0)  //��һ��
        {
            if(Vision_Dat_Explan() != 0) //�����⵽������
            {
                if(Run_Fast.new_flag == false) //�����û���жϳ���ǰ�Ƿ����³��ֵ�ͼ��
                {
                    if(Vision_Coord.Res_Coord[0].x < (Run_Fast.last_res.x-5)) //�����ǰ���������һ��������������ϴμ�¼�����һ������������Ҳ�
                    {
                        Run_Fast.new_flag = true; //��Ϊ��ǰ���������³��ֵ�����
                    }
                    else //���������������������ֵ���������������һ����������꣨��һֱ�������һ�����壩
                    {
                        Run_Fast.last_res.x = Vision_Coord.Res_Coord[0].x;
                        Run_Fast.last_res.y = Vision_Coord.Res_Coord[0].y;
                    }
                }
                else  //�Ѿ��жϳ���ǰ�����������������
                {
                    for(Run_Fast.i=(Vision_Coord.Res_Num-1); Run_Fast.i>=0; Run_Fast.i--)  //������ǰ���������������
                    {
                        if(Vision_Coord.Res_Coord[Run_Fast.i].x <= (Run_Fast.last_res.x - 5)) //�ҵ���һ������һ�����������һ�������Ҳ������
                        {
                            if(Run_Fast.i < (Vision_Coord.Res_Num-1)) //��������岻���⸱�����еĵ�һ������
                            {
                               Run_Fast.last_res.x = Vision_Coord.Res_Coord[Run_Fast.i+1].x; 
                               Run_Fast.last_res.y = Vision_Coord.Res_Coord[Run_Fast.i+1].y; //���¸���һ�¸��ٵ����һ�����������
                            }
                            else //�����������ǻ����еĵ�һ�����壨֤������һֱ���ٵ����һ�������Ѿ������˵�ǰ���棩
                            {
                                Run_Fast.last_res.x = 1365; //�����ٵ����һ�����������ֱ����0
                                Run_Fast.last_res.y = 767;
                            }
                            break; //�ҵ���һ������һ�����������һ�������Ҳ������֮��ֱ������forѭ�������ˣ�����Ҫ�ٱ����������
                        }
                    }
                }
            }
            if(Run_Fast.new_flag == true)  //����жϳ������µ�����
            {
                Run_Fast.user_num1 ++;
                if(Run_Fast.user_num1 >= 800) //��ʱ800ms֮������ͣ���ӻ���������������ͣ�ڵ��ӻ��Ҳ��Ե�����Ҳ��Ե���ܳ���ʶ���ȶ�����
                {
                    Run_Fast.user_num1 = 0; //���ʱ����Ʊ�����ֵ
                    Run_Fast.new_flag = false; //��ճ���������ı�־λ
                    Infared_Send_OK(); //���ͺ����ź���ͣ����
                    Run_Fast.step = 1; //��ǲ������ڶ���
                }
            }
        }

        if(Run_Fast.step == 1)  //�ڶ���
        {
            if(Vision_Dat_Explan() != 0)  //���AIģ���ܹ���⵽����
            {
                Run_Fast.user_num1++;
                //ͬ���ȱ������壬�ҵ���һ���³��ֵ����壨��Ϊ�ӵ�һ�������³������嵽��ͣ��800ms�����ʱ���ڿ��ܻ��³��ָ������壩
                for(Run_Fast.i=(Vision_Coord.Res_Num-1); Run_Fast.i>=0; Run_Fast.i--)  
                {
                    if(Vision_Coord.Res_Coord[Run_Fast.i].x < (Run_Fast.last_res.x - 5)) //�ҵ���һ���³��ֵ�����
                    {
                        if(Run_Fast.i < (Vision_Coord.Res_Num-1)) //���������岻����һ֡�����е�һ�������¸��¸��ٵ����һ�����������
                        {
                            Run_Fast.last_res.x = Vision_Coord.Res_Coord[Run_Fast.i+1].x; 
                            Run_Fast.last_res.y = Vision_Coord.Res_Coord[Run_Fast.i+1].y; 
                        }
                        else //��������������һ֡�����еĵ�һ���������ٵ����һ�������������Ϊ0
                        {
                            Run_Fast.last_res.x = 1365;
                            Run_Fast.last_res.y = 767;
                        }
                        break; //�ҵ���һ���³��ֵ�����֮��ֱ������ѭ��
                    }
                }
                if(Run_Fast.user_num1 >= 30) //������⵽30֮֡�󣨻������ͣʱͼ���ȶ�����ʶ��֡����30֡���ң�
                {
                    for(Run_Fast.i=0; Run_Fast.i<Vision_Coord.Res_Num; Run_Fast.i++) //ȡ��ͼ�������
                    {
                        Run_Fast.res_coord[Run_Fast.i].x = Vision_Coord.Res_Coord[Run_Fast.i].x;
                        Run_Fast.res_coord[Run_Fast.i].y = Vision_Coord.Res_Coord[Run_Fast.i].y;
                    }
                    Run_Fast.res_num = Vision_Coord.Res_Num; //��ֵͼ����������

                    for(Run_Fast.i=0; Run_Fast.i<Run_Fast.res_num; Run_Fast.i++) //���ֱ���ͼ�����꣬�����³��ֵ���Ʒ������
                    {
                        if(Run_Fast.res_coord[Run_Fast.i].x < Run_Fast.last_res.x) //��������һ֡���һ�������Ҳ�Ķ���Ϊ���µ�����
                        {
                            Run_Fast.need_point_num++;
                        }
                    }
                    Run_Fast.i = 0;
                    Run_Fast.step = 2; //��������Ϊ������
                    Run_Fast.user_num1 = 0;
                    Run_Fast.user_num2 = 0; //��������
                }
            }
            Run_Fast.user_num2++;
            //���������и��������30֡�������Ҫ1��Ϳ���������һ�������� Run_Fast.user_num2 �����������3000
            //���Run_Fast.user_num2 ����������3000����֤���������3�����޷������ȶ���⵽ͼ�������²�����Ƶ�����������ӵ�һ����ʼ
            if(Run_Fast.user_num2 >= 3000)
            {
                Run_Fast.user_num1 = 0;
                Run_Fast.user_num2 = 0; //��������
                Infared_Send_OK(); //���²�����Ƶ
                Run_Fast.step = 0; //��������
            }
        }
        if(Run_Fast.step == 2) //������ָ������
        {
            if((Run_Fast.user_num1 % 5000 == 0) && (Run_Fast.i < Run_Fast.need_point_num)) //5��һ������ָʾ����
            {
                Relay_Control(RELAY_OFF); //ָʾ֮ǰ�رռ����
                //��Ļ�ϴ�ӡ��⵽���������Ϣ
                Lcd_printf("Detected %s %s:x=%d,y=%d", ColorStr[select_color-1], ShapeStr[select_shape-1], Run_Fast.res_coord[Run_Fast.i].x, Run_Fast.res_coord[Run_Fast.i].y);
                DiWenLcd_Printf("��⵽%s%s: X=%d,Y=%d", ColorStrZh[select_color-1], ShapeStrZh[select_shape-1], Run_Fast.res_coord[Run_Fast.i].x, Run_Fast.res_coord[Run_Fast.i].y);
                Run_Fast.res_sum++; //��⵽�������������+1
				DiWenLcd_Cmd_Data(0x6004, Run_Fast.res_sum); //��������ʾ������Ϣ
                Tripod_X_Coord(Run_Fast.res_coord[Run_Fast.i].x);
                Tripod_Y_Coord(Run_Fast.res_coord[Run_Fast.i].y); //��̨����������ת��
                Voice_Play((select_shape-1)*4 + select_color); //����������⵽����
                while(Tripod_Y.is_running);
                while(Tripod_X.is_running);  //�ȴ���̨ת��ָ��λ��
                Relay_Control(RELAY_ON); //�򿪼����
                Run_Fast.i++;
                Run_Fast.user_num1 = 0;
            }

            Run_Fast.user_num1++;

            if(Run_Fast.user_num1 == 5001)
            {
                Relay_Control(RELAY_OFF); //ָʾ������رռ����
            }

            if(Run_Fast.user_num1 == 5500) //�ȴ�����������㣬�ص���ʼ״̬�����ⷢ���źţ����ӻ���������
            {
                Run_Fast.last_res.x = Run_Fast.res_coord[0].x; //�������һ�������x����
                Run_Fast.last_res.y = Run_Fast.res_coord[0].y;
                Infared_Send_OK();

                Run_Fast.user_num1 = 0;
                Run_Fast.step = 0;
                Run_Fast.i = 0;
                Run_Fast.need_point_num = 0;
            }
        }
    }
}

#endif



