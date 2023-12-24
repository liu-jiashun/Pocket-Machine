#include "run_stop.h"
#include "run_mode.h"
#include "lcd.h"
#include "voice.h"
#include "diwen_lcd.h"
#include "tripod.h"
#include "infared.h"
#include "relay.h"

/*ֹͣģʽ��ʼ��*/
void Run_Mode_Stop_Init(void)
{
    Run_Mode_Stop.mode_enter = Run_Mode_Stop_Enter; //������ʵ��ע�ᵽ����ָ��
    Run_Mode_Stop.mode_exit = Run_Mode_Stop_Exit;
    Run_Mode_Stop.mode_run = Run_Mode_Stop_Run;
}

/*ֹͣģʽ���뺯��*/
void Run_Mode_Stop_Enter(void)
{
    DiWenLcd_Show_RunState(129, "ֹͣ���"); //��������ʾ��ǰ״̬
    Voice_Play((uint8_t)STOP_DETECT); //��������
    Relay_Control(RELAY_OFF);
    Tripod_X_Coord(684);
    Tripod_Y_Coord(Y_OFFSET); //��̨��λ
    while(Tripod_Y.is_running);
    while(Tripod_X.is_running); //�ȴ���̨�˶�����
    Infared.Infared_Is_Pause = false;
}

/*ֹͣģʽ�˳�����*/
void Run_Mode_Stop_Exit(void)
{
}

/*ֹͣģʽ���к���*/
void Run_Mode_Stop_Run(void)
{
}


