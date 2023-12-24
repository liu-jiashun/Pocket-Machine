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

/*��̬ģʽ��ʼ������Ҫ�ǽ�����ָ��ָ�����ʵ�ֺ���������һЩ������ֵ��0*/
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
 

/*��̬ģʽ���뺯��*/
void Run_Mode_Static_Enter(void)
{
    Run_Static.step = 0;
    Run_Static.res_num = 0;
    Run_Static.code_corrd_x = 0;
    Run_Static.point_num = 0;
    Run_Static.same_flag = false;
    Run_Static.user_num = 0;

    Infared.Infared_Is_Pause = false;

    DiWenLcd_Show_RunState(129, "��̬ģʽ"); //��������ʾ����ģʽ

    /*��Ϊ�л�����������״����ɫ��ʱ��Ҳ�����һ��ģʽ���뺯������ʱ����Ҫ���Ž�������*/
    if(enter_voice_flag == true)  //�������������
    {
        Voice_Play((uint8_t)STATIC_START);  //���Ž�������ģʽ������
    }
    else  //����˴β�����������
    {
        enter_voice_flag = true;  //��ոñ�־λ
    }
}

/*��̬ģʽ�˳�����*/
void Run_Mode_Static_Exit(void)
{
}

/*��̬ģʽ���к���*/
void Run_Mode_Static_Run(void)
{
    if(Run_Static.step == 0)  //��һ������ѯAIģ���Ƿ��⵽ָ������
    {
        if(Vision_Dat_Explan() != 0)  //AIģ���⵽����
        {
            if(abs(Run_Static.code_corrd_x - Vision_Coord.Res_Coord[Vision_Coord.Res_Num-1].x)<STATIC_ALLOW)  //�����ǰ�ݴ�X�����ʶ�𵽵�����ƫ��������ķ�Χ��
            {
                Run_Static.user_num++; //����+1
                if(Run_Static.user_num >= 5) //������3����ͼ���Ѿ��ȶ���������һ�����ȴ�ͼ�������ȶ�����Ϊ���ӻ��л�ͼƬ��˲��ͼ�����ɫ����״����ʧ�棩
                {
                    Run_Static.step = 1;
                    Run_Static.user_num = 0;
                    Run_Static.code_corrd_x = 0;
                } 
            }
            else  //��������Χ
            {
                Run_Static.user_num = 0; //��������
                Run_Static.code_corrd_x = Vision_Coord.Res_Coord[Vision_Coord.Res_Num-1].x; //�ݴ�X����=��ǰ��⵽����������
            }
        }
    }

    if(Run_Static.step == 1)  //�ڶ������жϺ���һ֡�Ƿ���ͬһ��ͼƬ������ǰ�յ������һ������ͱ������һ֡���һ��������������Աȣ�
    {
        for(Run_Static.user_num=0; Run_Static.user_num<Vision_Coord.Res_Num; Run_Static.user_num++)
        {
            //������һ֡�Ƿ���ͬһ��ͼƬ�������֡��X��Y���궼����С������Ϊ��ͬһ��ͼƬ��
            Run_Static.same_flag = true;
            if((abs(Run_Static.res_coord[Run_Static.res_num-1].x - Vision_Coord.Res_Coord[Vision_Coord.Res_Num-1].x) > STATIC_ALLOW) || \
                (abs(Run_Static.res_coord[Run_Static.res_num-1].y - Vision_Coord.Res_Coord[Vision_Coord.Res_Num-1].y) > STATIC_ALLOW))
            {
                Run_Static.same_flag = false;
            }
        }
        if(Run_Static.same_flag == true) //�����ͬһ��ͼƬ
        {
            Run_Static.step = 0; //���ص�һ��
        }
        else //�������ͬһ��ͼƬ
        {
            for(Run_Static.user_num=0; Run_Static.user_num<Vision_Coord.Res_Num; Run_Static.user_num++)
            {
                Run_Static.res_coord[Run_Static.user_num].x = Vision_Coord.Res_Coord[Run_Static.user_num].x;
                Run_Static.res_coord[Run_Static.user_num].y = Vision_Coord.Res_Coord[Run_Static.user_num].y; //ȡ���������������
            }
            Run_Static.res_num = Vision_Coord.Res_Num;
            Run_Static.user_num = 0;
            Run_Static.step = 2; //���������
            Infared_Send_OK();  //���ⷢ���ź���ͣ���ӻ�      
        }
    }

    if(Run_Static.step == 2)  //�����������ݼ�⵽����Ʒ����������ָ������ 
    {
        if((Run_Static.user_num % 5000 == 0) && (Run_Static.point_num < Run_Static.res_num)) //5��һ��������ָ������
        {
            Relay_Control(RELAY_OFF); //ָʾ֮ǰ�رռ����
            Voice_Play((select_shape-1)*4 + select_color); //��������
            Tripod_X_Coord(Run_Static.res_coord[Run_Static.point_num].x);
            Tripod_Y_Coord(Run_Static.res_coord[Run_Static.point_num].y); //������̨X��Y��ת��
            while(Tripod_Y.is_running);
            while(Tripod_X.is_running); //�ȴ�ת������
            Relay_Control(RELAY_ON); //�򿪼����
            //��Ļ��ʾ�����
            Lcd_printf("Detected %s %s:x=%d,y=%d", ColorStr[select_color-1], ShapeStr[select_shape-1],  Run_Static.res_coord[Run_Static.point_num].x, Run_Static.res_coord[Run_Static.point_num].y);
            DiWenLcd_Printf("��⵽%s%s: X=%d,Y=%d", ColorStrZh[select_color-1], ShapeStrZh[select_shape-1], Run_Static.res_coord[Run_Static.point_num].x, Run_Static.res_coord[Run_Static.point_num].y);
            Run_Static.res_sum++; //��̬ģʽ�¼�⵽���������ļ���+1
			DiWenLcd_Cmd_Data(0x6002, Run_Static.res_sum); //����������ͼ�������
            Run_Static.point_num++;
            Run_Static.user_num = 0;
        }

        Run_Static.user_num++;

        if(Run_Static.user_num == 5001) //�������е��˴�˵����һ���жϵ�(Run_Static.point_num < Run_Static.res_num)�����Ѿ������㣬�����������Ѿ�ָʾ��
        {
            Relay_Control(RELAY_OFF); //���ȹرռ���ʣ���Ϊ����ʵ���������AIģ��������ʶ��
        }

        if(Run_Static.user_num == 5500) //�ٵȴ�0.5��֮��
        {
            Infared_Send_OK(); //���ⷢ���ź��õ��Ӽ�������
        }

        if(Run_Static.user_num == 6300)
        {
            Run_Static.step = 0; //�������㣨һ��ʶ���Ѿ���ɣ����¿�ʼ��
            Run_Static.user_num = 0;
            Run_Static.point_num = 0;
        }
    }
}


