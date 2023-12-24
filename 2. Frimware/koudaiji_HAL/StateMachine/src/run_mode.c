#include "run_mode.h"
#include "lcd.h"

Mode_Struct Run_Mode_Stop, Run_Mode_Static, Run_Mode_Slow, Run_Mode_Fast, Run_Mode_Manual; //����ģʽ�Ľṹ�������ֹͣģʽ����̬ģʽ������ģʽ������ģʽ���ֶ�ģʽ��
Mode_Struct *Run_Mode_Now;  //��ǰ����ģʽ��ָ��

bool enter_voice_flag = true;

/*����ģʽ��ʼ��*/
void Run_Mode_Init(void)
{
    Run_Mode_Stop_Init();
    Run_Mode_Static_Init();
    Run_Mode_Slow_Init();
    Run_Mode_Fast_Init();
    Run_Mode_Manual_Init();

    Run_Mode_Now  = &Run_Mode_Stop; //��ʼ״̬������ģʽĬ��Ϊֹͣģʽ
}



