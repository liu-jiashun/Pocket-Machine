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

/*����ģʽ��ʼ������Ҫ�ǽ�����ָ��ָ�����ʵ�ֺ���������һЩ������ֵ��0*/
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

/*����ģʽ���뺯��*/
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

    DiWenLcd_Show_RunState(129, "����ģʽ"); //��������ʾ����ģʽ

    /*��Ϊ�л�����������״����ɫ��ʱ��Ҳ�����һ��ģʽ���뺯������ʱ����Ҫ���Ž�������*/
    if(enter_voice_flag == true)  //�������������
    {
        Voice_Play((uint8_t)SLOW_START);  //���Ž�������ģʽ������
    }
    else  //����˴β�����������
    {
        enter_voice_flag = true;  //��ոñ�־λ
    }
}

/*����ģʽ�˳�����*/
void Run_Mode_Slow_Exit(void)
{
    Relay_Control(RELAY_OFF); //�رռ����
}

#ifdef SLOW_RIGHT_TO_LEFT

/*����ģʽ���к���*/
void Run_Mode_Slow_Run(void)
{
    /*�ж��Ƿ����״μ�⣨�״μ��ʱ��Ҫָʾ��Ļ�����м�⵽�����壬�Ժ���ֻ��Ҫָʾ�³��ֵ�����Ϳ��ԣ�*/
    if(Run_Slow.first_flag == true)  //�״μ�⵽������
    {
        if(Run_Slow.step == 0) //��һ�����ȴ���⵽����
        {
            if(Vision_Dat_Explan() != 0) //���AIģ���⵽����
            {
                Infared_Send_OK(); //��ͣ���ӻ�
                Run_Slow.step = 1; //������һ��
            }
        }
        if(Run_Slow.step == 1) //�ڶ�������֮ͣ���ж�ʶ��������Ƿ��ȶ����ڣ�
        {
            if(Vision_Dat_Explan() != 0) //�����⵽����
            {
                Run_Slow.user_num1++;
                if(Run_Slow.user_num1 >= 20) //����20�μ�⵽�������Ϊ������Ч�����ӽ��յ������ź�֮���з�Ӧʱ�䣬���ܴ�ʱ�����Ѿ��ߵ��˵��ӻ���Ե�����ʶ���ȶ�����ʶ�𲻵���
                {
                    Run_Slow.user_num1 = 0;
                    Run_Slow.user_num2 = 0;
                    Run_Slow.step = 2; //������һ��
                }
            }
            Run_Slow.user_num2++;
            if(Run_Slow.user_num2 >= 3000) //��֮ͣ��3����û�м�⵽����
            {
                Run_Slow.user_num1 = 0;
                Run_Slow.user_num2 = 0;
                Run_Slow.step = 0; //�ص���һ��
                Infared_Send_OK(); //���ӻ���������
            }
        }
        if(Run_Slow.step == 2) //������
        {
            for(Run_Slow.i=0; Run_Slow.i<Vision_Coord.Res_Num; Run_Slow.i++)
            {
                Run_Slow.res_coord[Run_Slow.i].x = Vision_Coord.Res_Coord[Run_Slow.i].x;
                Run_Slow.res_coord[Run_Slow.i].y = Vision_Coord.Res_Coord[Run_Slow.i].y;  //ȡ��ʶ�𵽵����е���������
            }
            Run_Slow.i = 0; //i����ʹ����֮������
            Run_Slow.res_num = Vision_Coord.Res_Num; //��ֵ��ǰ��Ʒ��Ŀ����
            Run_Slow.step = 3;       
        }

        if(Run_Slow.step == 3) //���Ĳ�������ָ������
        {
            if((Run_Slow.user_num1 % 5000 == 0) && (Run_Slow.i < Run_Slow.res_num)) //5��һ��������ָ������
            {
                Relay_Control(RELAY_OFF); //ָʾ֮ǰ�رռ����
                Tripod_X_Coord(Run_Slow.res_coord[Run_Slow.i].x);
                Tripod_Y_Coord(Run_Slow.res_coord[Run_Slow.i].y); //��̨�˶���ָ������
                Voice_Play((select_shape-1)*4 + select_color); //����������⵽����
                while(Tripod_Y.is_running);
                while(Tripod_X.is_running); //�ȴ���̨�˶�����
                Relay_Control(RELAY_ON); //�˶�����֮��򿪼����
                //��Ļ��ʾʶ�𵽵��������Ϣ
                Lcd_printf("Detected %s %s:x=%d,y=%d", ColorStr[select_color-1], ShapeStr[select_shape-1],  Run_Slow.res_coord[Run_Slow.i].x, Run_Slow.res_coord[Run_Slow.i].y);
                DiWenLcd_Printf("��⵽%s%s: X=%d,Y=%d", ColorStrZh[select_color-1], ShapeStrZh[select_shape-1], Run_Slow.res_coord[Run_Slow.i].x, Run_Slow.res_coord[Run_Slow.i].y);
                Run_Slow.res_sum++; //����ģʽ��ʶ�����������ļ���+1
				DiWenLcd_Cmd_Data(0x6004, Run_Slow.res_sum); //��������ʾʶ�����������
                Run_Slow.i++;
                Run_Slow.user_num1 = 0;
            }

            Run_Slow.user_num1++;

            if(Run_Slow.user_num1 == 5001) //�������е��˴�˵����һ���ж�(i < Run_Slow.res_num)���������㣬����������ָʾ���
            {
                Relay_Control(RELAY_OFF); //���ȹرռ���ʣ���Ϊ����ʻ����AIģ���ʶ��
            }

            if(Run_Slow.user_num1 == 5500) //�ȴ�0.5��֮��
            {
                Run_Slow.last_res.x = Run_Slow.res_coord[Run_Slow.res_num-1].x; //���浱ǰ���һ�������x����
                Run_Slow.last_res.y = Run_Slow.res_coord[Run_Slow.res_num-1].y;
                Infared_Send_OK(); //��������źţ����ӻ���������
                Run_Slow.step = 0; //�˴�ʶ��������ص���ʼ״̬
                Run_Slow.user_num1 = 0;
                Run_Slow.i = 0;
                Run_Slow.first_flag = false; //����״ν�����
            }
        }
    }
    /*�����״μ�������ʱ*/
    else
    {
        if(Run_Slow.step == 0)  //��һ��
        {
            if(Vision_Dat_Explan() != 0) //�����⵽������
            {
                if(Run_Slow.new_flag == false) //�����û���жϳ���ǰ�Ƿ����³��ֵ�ͼ��
                {
                    if(Vision_Coord.Res_Coord[Vision_Coord.Res_Num-1].x > (Run_Slow.last_res.x+5)) //�����ǰ���������һ��������������ϴμ�¼�����һ������������Ҳ�
                    {
                        Run_Slow.new_flag = true; //��Ϊ��ǰ���������³��ֵ�����
                    }
                    else //���������������������ֵ���������������һ����������꣨��һֱ�������һ�����壩
                    {
                        Run_Slow.last_res.x = Vision_Coord.Res_Coord[Vision_Coord.Res_Num-1].x;
                        Run_Slow.last_res.y = Vision_Coord.Res_Coord[Vision_Coord.Res_Num-1].y;
                    }
                }
                else  //�Ѿ��жϳ���ǰ�����������������
                {
                    for(Run_Slow.i=0; Run_Slow.i<Vision_Coord.Res_Num; Run_Slow.i++)  //������ǰ���������������
                    {
                        if(Vision_Coord.Res_Coord[Run_Slow.i].x >= (Run_Slow.last_res.x + 5)) //�ҵ���һ������һ�����������һ�������Ҳ������
                        {
                            if(Run_Slow.i > 0) //��������岻���⸱�����еĵ�һ������
                            {
                               Run_Slow.last_res.x = Vision_Coord.Res_Coord[Run_Slow.i-1].x; 
                               Run_Slow.last_res.y = Vision_Coord.Res_Coord[Run_Slow.i-1].y; //���¸���һ�¸��ٵ����һ�����������
                            }
                            else //�����������ǻ����еĵ�һ�����壨֤������һֱ���ٵ����һ�������Ѿ������˵�ǰ���棩
                            {
                                Run_Slow.last_res.x = 0; //�����ٵ����һ�����������ֱ����0
                                Run_Slow.last_res.y = 0;
                            }
                            break; //�ҵ���һ������һ�����������һ�������Ҳ������֮��ֱ������forѭ�������ˣ�����Ҫ�ٱ����������
                        }
                    }
                }
            }
            if(Run_Slow.new_flag == true)  //����жϳ������µ�����
            {
                Run_Slow.user_num1 ++;
                if(Run_Slow.user_num1 >= 800) //��ʱ800ms֮������ͣ���ӻ���������������ͣ�ڵ��ӻ��Ҳ��Ե�����Ҳ��Ե���ܳ���ʶ���ȶ�����
                {
                    Run_Slow.user_num1 = 0; //���ʱ����Ʊ�����ֵ
                    Run_Slow.new_flag = false; //��ճ���������ı�־λ
                    Infared_Send_OK(); //���ͺ����ź���ͣ����
                    Run_Slow.step = 1; //��ǲ������ڶ���
                }
            }
        }

        if(Run_Slow.step == 1)  //�ڶ���
        {
            if(Vision_Dat_Explan() != 0)  //���AIģ���ܹ���⵽����
            {
                Run_Slow.user_num1++;
                //ͬ���ȱ������壬�ҵ���һ���³��ֵ����壨��Ϊ�ӵ�һ�������³������嵽��ͣ��800ms�����ʱ���ڿ��ܻ��³��ָ������壩
                for(Run_Slow.i=0; Run_Slow.i<Vision_Coord.Res_Num; Run_Slow.i++)  
                {
                    if(Vision_Coord.Res_Coord[Run_Slow.i].x > (Run_Slow.last_res.x + 5)) //�ҵ���һ���³��ֵ�����
                    {
                        if(Run_Slow.i > 0) //���������岻����һ֡�����е�һ�������¸��¸��ٵ����һ�����������
                        {
                            Run_Slow.last_res.x = Vision_Coord.Res_Coord[Run_Slow.i-1].x; 
                            Run_Slow.last_res.y = Vision_Coord.Res_Coord[Run_Slow.i-1].y; 
                        }
                        else //��������������һ֡�����еĵ�һ���������ٵ����һ�������������Ϊ0
                        {
                            Run_Slow.last_res.x = 0;
                            Run_Slow.last_res.y = 0;
                        }
                        break; //�ҵ���һ���³��ֵ�����֮��ֱ������ѭ��
                    }
                }
                if(Run_Slow.user_num1 >= 30) //������⵽30֮֡�󣨻������ͣʱͼ���ȶ�����ʶ��֡����30֡���ң�
                {
                    for(Run_Slow.i=0; Run_Slow.i<Vision_Coord.Res_Num; Run_Slow.i++) //ȡ��ͼ�������
                    {
                        Run_Slow.res_coord[Run_Slow.i].x = Vision_Coord.Res_Coord[Run_Slow.i].x;
                        Run_Slow.res_coord[Run_Slow.i].y = Vision_Coord.Res_Coord[Run_Slow.i].y;
                    }
                    Run_Slow.res_num = Vision_Coord.Res_Num; //��ֵͼ����������

                    for(Run_Slow.i=0; Run_Slow.i<Run_Slow.res_num; Run_Slow.i++) //���ֱ���ͼ�����꣬�����³��ֵ���Ʒ������
                    {
                        if(Run_Slow.res_coord[Run_Slow.i].x > Run_Slow.last_res.x) //��������һ֡���һ�������Ҳ�Ķ���Ϊ���µ�����
                        {
                            Run_Slow.need_point_num++;
                        }
                    }
                    Run_Slow.i = 0;
                    Run_Slow.step = 2; //��������Ϊ������
                    Run_Slow.user_num1 = 0;
                    Run_Slow.user_num2 = 0; //��������
                }
            }
            Run_Slow.user_num2++;
            //���������и��������30֡�������Ҫ1��Ϳ���������һ�������� Run_Slow.user_num2 �����������3000
            //���Run_Slow.user_num2 ����������3000����֤���������3�����޷������ȶ���⵽ͼ�������²�����Ƶ�����������ӵ�һ����ʼ
            if(Run_Slow.user_num2 >= 3000)
            {
                Run_Slow.user_num1 = 0;
                Run_Slow.user_num2 = 0; //��������
                Infared_Send_OK(); //���²�����Ƶ
                Run_Slow.step = 0; //��������
            }
        }
        if(Run_Slow.step == 2) //������ָ������
        {
            if((Run_Slow.user_num1 % 5000 == 0) && (Run_Slow.i < Run_Slow.need_point_num)) //5��һ������ָʾ����
            {
                Relay_Control(RELAY_OFF); //ָʾ֮ǰ�رռ����
                //��Ļ�ϴ�ӡ��⵽���������Ϣ
                Lcd_printf("Detected %s %s:x=%d,y=%d", ColorStr[select_color-1], ShapeStr[select_shape-1], Run_Slow.res_coord[Run_Slow.res_num-1].x, Run_Slow.res_coord[Run_Slow.res_num-1].y);
                DiWenLcd_Printf("��⵽%s%s: X=%d,Y=%d", ColorStrZh[select_color-1], ShapeStrZh[select_shape-1], Run_Slow.res_coord[Run_Slow.res_num-1].x, Run_Slow.res_coord[Run_Slow.res_num-1].y);
                Run_Slow.res_sum++; //��⵽�������������+1
				DiWenLcd_Cmd_Data(0x6004, Run_Slow.res_sum); //��������ʾ������Ϣ
                Tripod_X_Coord(Run_Slow.res_coord[Run_Slow.res_num-Run_Slow.need_point_num+Run_Slow.i].x);
                Tripod_Y_Coord(Run_Slow.res_coord[Run_Slow.res_num-Run_Slow.need_point_num+Run_Slow.i].y); //��̨����������ת��
                Voice_Play((select_shape-1)*4 + select_color); //����������⵽����
                while(Tripod_Y.is_running);
                while(Tripod_X.is_running);  //�ȴ���̨ת��ָ��λ��
                Relay_Control(RELAY_ON); //�򿪼����
                Run_Slow.i++;
                Run_Slow.user_num1 = 0;
            }

            Run_Slow.user_num1++;

            if(Run_Slow.user_num1 == 5001)
            {
                Relay_Control(RELAY_OFF); //ָʾ������رռ����
            }

            if(Run_Slow.user_num1 == 5500) //�ȴ�����������㣬�ص���ʼ״̬�����ⷢ���źţ����ӻ���������
            {
                Run_Slow.last_res.x = Run_Slow.res_coord[Run_Slow.res_num-1].x; //�������һ�������x����
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
//�������Ƶ�������²��ţ��߼�������һ����ֻ����������жϸĳɶԱ�Y���ꡣ
#ifdef SLOW_UP_TO_DOWN

/*����ģʽ���к���*/
void Run_Mode_Slow_Run(void)
{
    /*�ж��Ƿ����״μ�⣨�״μ��ʱ��Ҫָʾ��Ļ�����м�⵽�����壬�Ժ���ֻ��Ҫָʾ���һ���Ϳ��ԣ�*/
    if(Run_Slow.first_flag == true)  //�״μ�⵽������
    {
        if(Run_Slow.step == 0) //��һ�����ȴ���⵽����
        {
            if(Vision_Dat_Explan() != 0) //���AIģ���⵽����
            {
                Infared_Send_OK(); //��ͣ���ӻ�
                Run_Slow.step = 1; //������һ��
            }
        }
        if(Run_Slow.step == 1) //�ڶ���
        {
            if(Vision_Dat_Explan() != 0) //�����⵽����
            {
                Run_Slow.user_num1++;
                if(Run_Slow.user_num1 >= 20) //����20�μ�⵽�������Ϊ������Ч�����ӽ��յ������ź�֮���з�Ӧʱ�䣬���ܴ�ʱ�����Ѿ��ߵ��˵��ӻ���Ե�����ʶ���ȶ�����ʶ�𲻵���
                {
                    Run_Slow.user_num1 = 0;
                    Run_Slow.user_num2 = 0;
                    Run_Slow.step = 2; //������һ��
                }
            }
            Run_Slow.user_num2++;
            if(Run_Slow.user_num2 >= 3000) //��֮ͣ��3����û�м�⵽����
            {
                Run_Slow.user_num1 = 0;
                Run_Slow.user_num2 = 0;
                Run_Slow.step = 0; //�ص���һ��
                Infared_Send_OK(); //���ӻ���������
            }
        }
        if(Run_Slow.step == 2) //������
        {
            for(Run_Slow.i=0; Run_Slow.i<Vision_Coord.Res_Num; Run_Slow.i++)
            {
                Run_Slow.res_coord[Run_Slow.i].x = Vision_Coord.Res_Coord[Run_Slow.i].x;
                Run_Slow.res_coord[Run_Slow.i].y = Vision_Coord.Res_Coord[Run_Slow.i].y;  //ȡ��ʶ�𵽵����е���������
            }
            Run_Slow.i = 0;
            Run_Slow.res_num = Vision_Coord.Res_Num;
            Run_Slow.step = 3;       
        }

        if(Run_Slow.step == 3) //���Ĳ�������ָ������
        {
            if((Run_Slow.user_num1 % 5000 == 0) && (Run_Slow.i < Run_Slow.res_num)) //5��һ��������ָ������
            {
                Relay_Control(RELAY_OFF); //ָʾ֮ǰ�رռ����
                Tripod_X_Coord(Run_Slow.res_coord[Run_Slow.i].x);
                Tripod_Y_Coord(Run_Slow.res_coord[Run_Slow.i].y); //��̨�˶���ָ������
                Voice_Play((select_shape-1)*4 + select_color); //����������⵽����
                while(Tripod_Y.is_running);
                while(Tripod_X.is_running); //�ȴ���̨�˶�����
                Relay_Control(RELAY_ON); //�˶�����֮��򿪼����
                //��Ļ��ʾʶ�𵽵��������Ϣ
                Lcd_printf("Detected %s %s:x=%d,y=%d", ColorStr[select_color-1], ShapeStr[select_shape-1],  Run_Slow.res_coord[Run_Slow.i].x, Run_Slow.res_coord[Run_Slow.i].y);
                DiWenLcd_Printf("��⵽%s%s: X=%d,Y=%d", ColorStrZh[select_color-1], ShapeStrZh[select_shape-1], Run_Slow.res_coord[Run_Slow.i].x, Run_Slow.res_coord[Run_Slow.i].y);
                Run_Slow.res_sum++; //����ģʽ��ʶ�����������ļ���+1
				DiWenLcd_Cmd_Data(0x6004, Run_Slow.res_sum); //��������ʾʶ�����������
                Run_Slow.i++;
                Run_Slow.user_num1 = 0;
            }

            Run_Slow.user_num1++;

            if(Run_Slow.user_num1 == 5001) //�������е��˴�˵����һ���ж�(i < Run_Slow.res_num)���������㣬����������ָʾ���
            {
                Relay_Control(RELAY_OFF); //���ȹرռ���ʣ���Ϊ����ʻ����AIģ���ʶ��
            }

            if(Run_Slow.user_num1 == 5500) //�ȴ�1��֮��
            {
                Run_Slow.last_res.x = Run_Slow.res_coord[Run_Slow.res_num-1].x; //���浱ǰ���һ�������x����
                Run_Slow.last_res.y = Run_Slow.res_coord[Run_Slow.res_num-1].y;
                Infared_Send_OK(); //��������źţ����ӻ���������
                Run_Slow.step = 0; //�˴�ʶ��������ص���ʼ״̬
                Run_Slow.user_num1 = 0;
                Run_Slow.i = 0;
                Run_Slow.first_flag = false; //����״ν�����
            }
        }
    }
    /*�����״μ�������ʱ*/
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
        if(Run_Slow.step == 2) //������ָ������
        {
            if((Run_Slow.user_num1 % 5000 == 0) && (Run_Slow.i < Run_Slow.need_point_num))
            {
                Relay_Control(RELAY_OFF); //ָʾ֮ǰ�رռ����
                //��Ļ�ϴ�ӡ��⵽���������Ϣ
                Lcd_printf("Detected %s %s:x=%d,y=%d", ColorStr[select_color-1], ShapeStr[select_shape-1], Run_Slow.res_coord[Run_Slow.res_num-Run_Slow.need_point_num+Run_Slow.i].x, Run_Slow.res_coord[Run_Slow.res_num-Run_Slow.need_point_num+Run_Slow.i].y);
                DiWenLcd_Printf("��⵽%s%s: X=%d,Y=%d", ColorStrZh[select_color-1], ShapeStrZh[select_shape-1], Run_Slow.res_coord[Run_Slow.res_num-Run_Slow.need_point_num+Run_Slow.i].x, Run_Slow.res_coord[Run_Slow.res_num-Run_Slow.need_point_num+Run_Slow.i].y);
                Run_Slow.res_sum++; //��⵽�������������+1
				DiWenLcd_Cmd_Data(0x6004, Run_Slow.res_sum); //��������ʾ������Ϣ
                Tripod_X_Coord(Run_Slow.res_coord[Run_Slow.res_num-Run_Slow.need_point_num+Run_Slow.i].x);
                Tripod_Y_Coord(Run_Slow.res_coord[Run_Slow.res_num-Run_Slow.need_point_num+Run_Slow.i].y); //��̨����������ת��
                Voice_Play((select_shape-1)*4 + select_color); //����������⵽����
                while(Tripod_Y.is_running);
                while(Tripod_X.is_running);  //�ȴ���̨ת��ָ��λ��
                Relay_Control(RELAY_ON); //�򿪼����
                Run_Slow.i++;
                Run_Slow.user_num1 = 0;
            }

            Run_Slow.user_num1++;

            if(Run_Slow.user_num1 == 5001)
            {
                Relay_Control(RELAY_OFF); //ָʾ������رռ����
            }

            if(Run_Slow.user_num1 == 5500) //�ȴ�һ��������㣬�ص���ʼ״̬�����ⷢ���źţ����ӻ���������
            {
                Run_Slow.last_res.x = Run_Slow.res_coord[Run_Slow.res_num-1].x; //�������һ�������x����
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

/*����ģʽ���к���*/
void Run_Mode_Slow_Run(void)
{
    /*�ж��Ƿ����״μ�⣨�״μ��ʱ��Ҫָʾ��Ļ�����м�⵽�����壬�Ժ���ֻ��Ҫָʾ�³��ֵ�����Ϳ��ԣ�*/
    if(Run_Slow.first_flag == true)  //�״μ�⵽������
    {
        if(Run_Slow.step == 0) //��һ�����ȴ���⵽����
        {
            if(Vision_Dat_Explan() != 0) //���AIģ���⵽����
            {
                Infared_Send_OK(); //��ͣ���ӻ�
                Run_Slow.step = 1; //������һ��
            }
        }
        if(Run_Slow.step == 1) //�ڶ�������֮ͣ���ж�ʶ��������Ƿ��ȶ����ڣ�
        {
            if(Vision_Dat_Explan() != 0) //�����⵽����
            {
                Run_Slow.user_num1++;
                if(Run_Slow.user_num1 >= 20) //����20�μ�⵽�������Ϊ������Ч�����ӽ��յ������ź�֮���з�Ӧʱ�䣬���ܴ�ʱ�����Ѿ��ߵ��˵��ӻ���Ե�����ʶ���ȶ�����ʶ�𲻵���
                {
                    Run_Slow.user_num1 = 0;
                    Run_Slow.user_num2 = 0;
                    Run_Slow.step = 2; //������һ��
                }
            }
            Run_Slow.user_num2++;
            if(Run_Slow.user_num2 >= 3000) //��֮ͣ��3����û�м�⵽����
            {
                Run_Slow.user_num1 = 0;
                Run_Slow.user_num2 = 0;
                Run_Slow.step = 0; //�ص���һ��
                Infared_Send_OK(); //���ӻ���������
            }
        }
        if(Run_Slow.step == 2) //������
        {
            for(Run_Slow.i=0; Run_Slow.i<Vision_Coord.Res_Num; Run_Slow.i++)
            {
                Run_Slow.res_coord[Run_Slow.i].x = Vision_Coord.Res_Coord[Run_Slow.i].x;
                Run_Slow.res_coord[Run_Slow.i].y = Vision_Coord.Res_Coord[Run_Slow.i].y;  //ȡ��ʶ�𵽵����е���������
            }
            Run_Slow.i = 0; //i����ʹ����֮������
            Run_Slow.res_num = Vision_Coord.Res_Num; //��ֵ��ǰ��Ʒ��Ŀ����
            Run_Slow.step = 3;       
        }

        if(Run_Slow.step == 3) //���Ĳ�������ָ������
        {
            if((Run_Slow.user_num1 % 5000 == 0) && (Run_Slow.i < Run_Slow.res_num)) //5��һ��������ָ������
            {
                Relay_Control(RELAY_OFF); //ָʾ֮ǰ�رռ����
                Tripod_X_Coord(Run_Slow.res_coord[Run_Slow.i].x);
                Tripod_Y_Coord(Run_Slow.res_coord[Run_Slow.i].y); //��̨�˶���ָ������
                Voice_Play((select_shape-1)*4 + select_color); //����������⵽����
                while(Tripod_Y.is_running);
                while(Tripod_X.is_running); //�ȴ���̨�˶�����
                Relay_Control(RELAY_ON); //�˶�����֮��򿪼����
                //��Ļ��ʾʶ�𵽵��������Ϣ
                Lcd_printf("Detected %s %s:x=%d,y=%d", ColorStr[select_color-1], ShapeStr[select_shape-1],  Run_Slow.res_coord[Run_Slow.i].x, Run_Slow.res_coord[Run_Slow.i].y);
                DiWenLcd_Printf("��⵽%s%s: X=%d,Y=%d", ColorStrZh[select_color-1], ShapeStrZh[select_shape-1], Run_Slow.res_coord[Run_Slow.i].x, Run_Slow.res_coord[Run_Slow.i].y);
                Run_Slow.res_sum++; //����ģʽ��ʶ�����������ļ���+1
				DiWenLcd_Cmd_Data(0x6004, Run_Slow.res_sum); //��������ʾʶ�����������
                Run_Slow.i++;
                Run_Slow.user_num1 = 0;
            }

            Run_Slow.user_num1++;

            if(Run_Slow.user_num1 == 5001) //�������е��˴�˵����һ���ж�(i < Run_Slow.res_num)���������㣬����������ָʾ���
            {
                Relay_Control(RELAY_OFF); //���ȹرռ���ʣ���Ϊ����ʻ����AIģ���ʶ��
            }

            if(Run_Slow.user_num1 == 5500) //�ȴ�0.5��֮��
            {
                Run_Slow.last_res.x = Run_Slow.res_coord[0].x; //���浱ǰ���һ�������x����
                Run_Slow.last_res.y = Run_Slow.res_coord[0].y;
                Infared_Send_OK(); //��������źţ����ӻ���������
                Run_Slow.step = 0; //�˴�ʶ��������ص���ʼ״̬
                Run_Slow.user_num1 = 0;
                Run_Slow.i = 0;
                Run_Slow.first_flag = false; //����״ν�����
            }
        }
    }
    /*�����״μ�������ʱ*/
    else
    {
        if(Run_Slow.step == 0)  //��һ��
        {
            if(Vision_Dat_Explan() != 0) //�����⵽������
            {
                if(Run_Slow.new_flag == false) //�����û���жϳ���ǰ�Ƿ����³��ֵ�ͼ��
                {
                    if(Vision_Coord.Res_Coord[0].x < (Run_Slow.last_res.x-5)) //�����ǰ���������һ��������������ϴμ�¼�����һ������������Ҳ�
                    {
                        Run_Slow.new_flag = true; //��Ϊ��ǰ���������³��ֵ�����
                    }
                    else //���������������������ֵ���������������һ����������꣨��һֱ�������һ�����壩
                    {
                        Run_Slow.last_res.x = Vision_Coord.Res_Coord[0].x;
                        Run_Slow.last_res.y = Vision_Coord.Res_Coord[0].y;
                    }
                }
                else  //�Ѿ��жϳ���ǰ�����������������
                {
                    for(Run_Slow.i=(Vision_Coord.Res_Num-1); Run_Slow.i>=0; Run_Slow.i--)  //������ǰ���������������
                    {
                        if(Vision_Coord.Res_Coord[Run_Slow.i].x <= (Run_Slow.last_res.x - 5)) //�ҵ���һ������һ�����������һ�������Ҳ������
                        {
                            if(Run_Slow.i < (Vision_Coord.Res_Num-1)) //��������岻���⸱�����еĵ�һ������
                            {
                               Run_Slow.last_res.x = Vision_Coord.Res_Coord[Run_Slow.i+1].x; 
                               Run_Slow.last_res.y = Vision_Coord.Res_Coord[Run_Slow.i+1].y; //���¸���һ�¸��ٵ����һ�����������
                            }
                            else //�����������ǻ����еĵ�һ�����壨֤������һֱ���ٵ����һ�������Ѿ������˵�ǰ���棩
                            {
                                Run_Slow.last_res.x = 1365; //�����ٵ����һ�����������ֱ����0
                                Run_Slow.last_res.y = 767;
                            }
                            break; //�ҵ���һ������һ�����������һ�������Ҳ������֮��ֱ������forѭ�������ˣ�����Ҫ�ٱ����������
                        }
                    }
                }
            }
            if(Run_Slow.new_flag == true)  //����жϳ������µ�����
            {
                Run_Slow.user_num1 ++;
                if(Run_Slow.user_num1 >= 800) //��ʱ800ms֮������ͣ���ӻ���������������ͣ�ڵ��ӻ��Ҳ��Ե�����Ҳ��Ե���ܳ���ʶ���ȶ�����
                {
                    Run_Slow.user_num1 = 0; //���ʱ����Ʊ�����ֵ
                    Run_Slow.new_flag = false; //��ճ���������ı�־λ
                    Infared_Send_OK(); //���ͺ����ź���ͣ����
                    Run_Slow.step = 1; //��ǲ������ڶ���
                }
            }
        }

        if(Run_Slow.step == 1)  //�ڶ���
        {
            if(Vision_Dat_Explan() != 0)  //���AIģ���ܹ���⵽����
            {
                Run_Slow.user_num1++;
                //ͬ���ȱ������壬�ҵ���һ���³��ֵ����壨��Ϊ�ӵ�һ�������³������嵽��ͣ��800ms�����ʱ���ڿ��ܻ��³��ָ������壩
                for(Run_Slow.i=(Vision_Coord.Res_Num-1); Run_Slow.i>=0; Run_Slow.i--)  
                {
                    if(Vision_Coord.Res_Coord[Run_Slow.i].x < (Run_Slow.last_res.x - 5)) //�ҵ���һ���³��ֵ�����
                    {
                        if(Run_Slow.i < (Vision_Coord.Res_Num-1)) //���������岻����һ֡�����е�һ�������¸��¸��ٵ����һ�����������
                        {
                            Run_Slow.last_res.x = Vision_Coord.Res_Coord[Run_Slow.i+1].x; 
                            Run_Slow.last_res.y = Vision_Coord.Res_Coord[Run_Slow.i+1].y; 
                        }
                        else //��������������һ֡�����еĵ�һ���������ٵ����һ�������������Ϊ0
                        {
                            Run_Slow.last_res.x = 1365;
                            Run_Slow.last_res.y = 767;
                        }
                        break; //�ҵ���һ���³��ֵ�����֮��ֱ������ѭ��
                    }
                }
                if(Run_Slow.user_num1 >= 30) //������⵽30֮֡�󣨻������ͣʱͼ���ȶ�����ʶ��֡����30֡���ң�
                {
                    for(Run_Slow.i=0; Run_Slow.i<Vision_Coord.Res_Num; Run_Slow.i++) //ȡ��ͼ�������
                    {
                        Run_Slow.res_coord[Run_Slow.i].x = Vision_Coord.Res_Coord[Run_Slow.i].x;
                        Run_Slow.res_coord[Run_Slow.i].y = Vision_Coord.Res_Coord[Run_Slow.i].y;
                    }
                    Run_Slow.res_num = Vision_Coord.Res_Num; //��ֵͼ����������

                    for(Run_Slow.i=0; Run_Slow.i<Run_Slow.res_num; Run_Slow.i++) //���ֱ���ͼ�����꣬�����³��ֵ���Ʒ������
                    {
                        if(Run_Slow.res_coord[Run_Slow.i].x < Run_Slow.last_res.x) //��������һ֡���һ�������Ҳ�Ķ���Ϊ���µ�����
                        {
                            Run_Slow.need_point_num++;
                        }
                    }
                    Run_Slow.i = 0;
                    Run_Slow.step = 2; //��������Ϊ������
                    Run_Slow.user_num1 = 0;
                    Run_Slow.user_num2 = 0; //��������
                }
            }
            Run_Slow.user_num2++;
            //���������и��������30֡�������Ҫ1��Ϳ���������һ�������� Run_Slow.user_num2 �����������3000
            //���Run_Slow.user_num2 ����������3000����֤���������3�����޷������ȶ���⵽ͼ�������²�����Ƶ�����������ӵ�һ����ʼ
            if(Run_Slow.user_num2 >= 3000)
            {
                Run_Slow.user_num1 = 0;
                Run_Slow.user_num2 = 0; //��������
                Infared_Send_OK(); //���²�����Ƶ
                Run_Slow.step = 0; //��������
            }
        }
        if(Run_Slow.step == 2) //������ָ������
        {
            if((Run_Slow.user_num1 % 5000 == 0) && (Run_Slow.i < Run_Slow.need_point_num)) //5��һ������ָʾ����
            {
                Relay_Control(RELAY_OFF); //ָʾ֮ǰ�رռ����
                //��Ļ�ϴ�ӡ��⵽���������Ϣ
                Lcd_printf("Detected %s %s:x=%d,y=%d", ColorStr[select_color-1], ShapeStr[select_shape-1], Run_Slow.res_coord[Run_Slow.i].x, Run_Slow.res_coord[Run_Slow.i].y);
                DiWenLcd_Printf("��⵽%s%s: X=%d,Y=%d", ColorStrZh[select_color-1], ShapeStrZh[select_shape-1], Run_Slow.res_coord[Run_Slow.i].x, Run_Slow.res_coord[Run_Slow.i].y);
                Run_Slow.res_sum++; //��⵽�������������+1
				DiWenLcd_Cmd_Data(0x6004, Run_Slow.res_sum); //��������ʾ������Ϣ
                Tripod_X_Coord(Run_Slow.res_coord[Run_Slow.i].x);
                Tripod_Y_Coord(Run_Slow.res_coord[Run_Slow.i].y); //��̨����������ת��
                Voice_Play((select_shape-1)*4 + select_color); //����������⵽����
                while(Tripod_Y.is_running);
                while(Tripod_X.is_running);  //�ȴ���̨ת��ָ��λ��
                Relay_Control(RELAY_ON); //�򿪼����
                Run_Slow.i++;
                Run_Slow.user_num1 = 0;
            }

            Run_Slow.user_num1++;

            if(Run_Slow.user_num1 == 5001)
            {
                Relay_Control(RELAY_OFF); //ָʾ������رռ����
            }

            if(Run_Slow.user_num1 == 5500) //�ȴ�����������㣬�ص���ʼ״̬�����ⷢ���źţ����ӻ���������
            {
                Run_Slow.last_res.x = Run_Slow.res_coord[0].x; //�������һ�������x����
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



