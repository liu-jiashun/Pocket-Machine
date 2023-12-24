/*
*���ļ��ڵĳ�����Ҫʵ������İ����߼�
*/
#include "logic.h"
#include "matrix_key.h"
#include "touch_key.h"
#include "vision.h"
#include "beep.h"
#include "infared.h"
#include "lcd.h"
#include "diwen_lcd.h"
#include "relay.h"
#include "tripod.h"
#include "run_mode.h"
#include "voice.h"
#include "led.h"
#include "vision.h"

uint8_t select_color = 1;
uint8_t select_shape = 1;

volatile bool manual_claib_finish = false;  //��̨������ͷ�ֶ�У׼��ɱ�־

uint8_t ColorStr[][8] = {"Red","Green","Blue","Yellow"};
uint8_t ShapeStr[][16] = {"Square","Circle","Triangle","Annulus","Pentagon"};
uint8_t ColorStrZh[][8] = {"��ɫ","��ɫ","��ɫ","��ɫ"};
uint8_t ShapeStrZh[][16] = {"������","Բ��","������","Բ��","�ı���"};  /*��״����ɫ����Ӣ���ַ�*/

/*����ģʽ�л��������߼�ʵ��*/
void Logic_Matrix_Key(void)
{
    if(Matrix_Key_Value != MATRIX_KEY_NONE)
    {
        switch (Matrix_Key_Value)
        {
            case MATRIX_KEY_F1:  //F1�������½��뾲̬ģʽ
                if(Run_Mode_Now == &Run_Mode_Stop)  //�����ʱ��ֹͣ״̬
                {
                    Run_Mode_Now->mode_exit(); //�˳���ǰ״̬
                    Run_Mode_Now = &Run_Mode_Static; //���õ�ǰ����ģʽ�Ǿ�̬ģʽ
                    Run_Mode_Now->mode_enter(); //���뾲̬ģʽ
                }
                else if(Run_Mode_Now == &Run_Mode_Static)
                {
                    Run_Mode_Now->mode_exit(); //�˳���ǰ״̬
                    Run_Mode_Now = &Run_Mode_Stop; //���õ�ǰ����ģʽ��ֹͣģʽ
                    Run_Mode_Now->mode_enter(); //����ֹͣģʽ
                }
                Matrix_Key_Value = MATRIX_KEY_NONE; //��հ�����ֵ
            break;

            case MATRIX_KEY_F2:  //F2�������½�������ģʽ���߼�ͬ��̬ģʽһ��
                if(Run_Mode_Now == &Run_Mode_Stop)
                {
                    Run_Mode_Now->mode_exit();
                    Run_Mode_Now = &Run_Mode_Slow;
                    Run_Mode_Now->mode_enter();
                }
                else if(Run_Mode_Now == &Run_Mode_Slow)
                {
                    Run_Mode_Now->mode_exit(); //�˳���ǰ״̬
                    Run_Mode_Now = &Run_Mode_Stop; //���õ�ǰ����ģʽ��ֹͣģʽ
                    Run_Mode_Now->mode_enter(); //����ֹͣģʽ
                }
                Matrix_Key_Value = MATRIX_KEY_NONE;
            break;

            case MATRIX_KEY_F3:  //F3�������½������ģʽ���߼�ͬ��̬ģʽһ��
                if(Run_Mode_Now == &Run_Mode_Stop)
                {
                    Run_Mode_Now->mode_exit();
                    Run_Mode_Now = &Run_Mode_Fast;
                    Run_Mode_Now->mode_enter();
                }
                else if(Run_Mode_Now == &Run_Mode_Fast)
                {
                    Run_Mode_Now->mode_exit(); //�˳���ǰ״̬
                    Run_Mode_Now = &Run_Mode_Stop; //���õ�ǰ����ģʽ��ֹͣģʽ
                    Run_Mode_Now->mode_enter(); //����ֹͣģʽ
                }
                Matrix_Key_Value = MATRIX_KEY_NONE;
            break;

            case MATRIX_KEY_F4:  //F4�������½����ֶ�ģʽ
                if(Run_Mode_Now == &Run_Mode_Stop)
                {
                    Run_Mode_Now->mode_exit();
                    Run_Mode_Now = &Run_Mode_Manual;
                    Run_Mode_Now->mode_enter();
                }
                else if(Run_Mode_Now == &Run_Mode_Manual)
                {
                    Run_Mode_Now->mode_exit(); //�˳���ǰ״̬
                    Run_Mode_Now = &Run_Mode_Stop; //���õ�ǰ����ģʽ��ֹͣģʽ
                    Run_Mode_Now->mode_enter(); //����ֹͣģʽ
                }
                Matrix_Key_Value = MATRIX_KEY_NONE;
            break;

            case MATRIX_KEY_E:  //E�������£��˳���ǰ���е�ģʽ
                if(Run_Mode_Now != &Run_Mode_Stop)  //��ǰ����ֹͣ״̬
                {
                    Run_Mode_Now->mode_exit();
                    Run_Mode_Now = &Run_Mode_Stop;
                    Run_Mode_Now->mode_enter();
                }
                Matrix_Key_Value = MATRIX_KEY_NONE;
            break;

            default:
            break;
        }
    }
}

/*���������������߼�ʵ��*/
void Logic_Touch_Key(void)
{
    if(Touch_Key_Value != TOUCH_KEY_NONE)
    {
        switch (Touch_Key_Value)
        {
            case TOUCH_KEY_A:  //����A�л�������ɫ
                if(Infared.Infared_Is_Pause == false) //��ǰû�м�⵽����ʱ�����л�
                {
                    select_color++;
                    if(select_color>4)select_color=1; //������ɫѭ���л�
                    Vision_Send_Cmd(select_color, select_shape); //��AI���ģ�鷢�ͼ��ָ��

                    Lcd_printf("Camera_Set:color=%s shape=%s", ColorStr[select_color-1], ShapeStr[select_shape-1]);
                    DiWenLcd_Show_RunState(130, "%s%s", ColorStrZh[select_color-1], ShapeStrZh[select_shape-1]); //��Ļ��ʾ��ǰ��������

                    if((Run_Mode_Now == &Run_Mode_Slow) || (Run_Mode_Now == &Run_Mode_Fast) || (Run_Mode_Now == &Run_Mode_Static)) //����ɫ�л�֮����Ҫ��״̬��λ��������һ��״̬���뺯������
                    {
                        enter_voice_flag = false; //״̬����ʱ�Ƿ񲥱�������Ч��־-�л���ɫ����ʱ������
                        Run_Mode_Now->mode_enter();
                    }
                }
                else  //��ǰ�Ѿ���⵽����ʱ����Ļ��ʾ�����л�
                {
                    Lcd_printf("RunStage_Detected can't change");
                    DiWenLcd_Printf("��λ״̬���޷��л�");
                }
            break;

            case TOUCH_KEY_B:  //����B�л�������״���߼�ͬ����A��ͬ
                if(Infared.Infared_Is_Pause == false)
                {
                    select_shape++;
                    if(select_shape>5)select_shape=1;
                    Vision_Send_Cmd(select_color, select_shape);
                    Lcd_printf("Camera_Set:color=%s shape=%s", ColorStr[select_color-1], ShapeStr[select_shape-1]);
                    DiWenLcd_Show_RunState(130, "%s%s", ColorStrZh[select_color-1], ShapeStrZh[select_shape-1]);

                    if((Run_Mode_Now == &Run_Mode_Slow) || (Run_Mode_Now == &Run_Mode_Fast) || (Run_Mode_Now == &Run_Mode_Static))
                    {
                        enter_voice_flag = false;
                        Run_Mode_Now->mode_enter();
                    } 
                }
                else
                {
                    Lcd_printf("RunStage_Detected can't change");
                    DiWenLcd_Printf("��λ״̬���޷��л�");
                }
            break;

            case TOUCH_KEY_C:
            break;

            case TOUCH_KEY_D:  //����D�л��ֶ�ģʽ����
                if(Run_Manual.mode == 0)
                {
                    Run_Manual.mode = 1;
                    Lcd_printf("dist mode");
                    Run_Manual.target_x = 0;
                    Run_Manual.target_y = 0;
                    Run_Manual.step = 0;
                }
                else
                {
                    Run_Manual.mode = 0;
                    Lcd_printf("coord mode");
                    Run_Manual.target_x = 0;
                    Run_Manual.target_y = 0;
                    Run_Manual.step = 0;
                }
            break;

            default:
            break;
        }

        Touch_Key_Value = TOUCH_KEY_NONE; //������ֵ����
    }
}

/*����������̨У׼����߼�*/
void Logic_Tripod_Calib(void)
{
    static uint16_t offset;
    static bool vision_clib_flag = false;
    static bool set_offset_finish = false;
    static bool is_manual_ing = false;

    if(Touch_Key_Value != TOUCH_KEY_NONE)
    {
        switch (Touch_Key_Value)
        {
            case TOUCH_KEY_UP: //�����������ϼ���������̨Y��У׼
			{
                static uint8_t num;
                num++;
                if(num > 2) num = 0;
                if(num == 0) Tripod_Y_Coord(Y_OFFSET);
                if(num == 1) Tripod_Y_Coord(0);
                if(num == 2) Tripod_Y_Coord((DIS_Y-1));  //ѭ��ָʾ���ӻ��м䡢�ϱ��ء��±���
			}
            break;

            case TOUCH_KEY_LEFT: //���������������������̨X��У׼
			{
                static uint8_t num;
                num++;
                if(num > 2) num = 0;
                if(num == 0) Tripod_X_Coord(684);
                if(num == 1) Tripod_X_Coord(0);
                if(num == 2) Tripod_X_Coord(1365);  //ѭ��ָʾ���ӻ��м䣬����أ��ұ���
			}
            break;

            case TOUCH_KEY_DOWN: //�����������¼������о�ͷ�Ľ�У׼
            {
                select_color = 1;
                select_shape = 1;
                Vision_Send_Cmd(select_color, select_shape);
                HAL_Delay(200);
                Vision_Send_Cmd(select_color, select_shape);
                vision_clib_flag = true;
                Lcd_printf("Vision Clib");
            }
            break;

            case TOUCH_KEY_RIGHT: //�����������Ҽ��������̨�ֶ�У׼���
            {
                if(set_offset_finish == true)
                {
                    manual_claib_finish = true;
                    Relay_Control(RELAY_OFF);
                    vision_clib_flag = false;
                }
                else
                {
                    Lcd_printf("input offset!!");
                }
            }
            break;

            default:
            break;
        }

        Touch_Key_Value = TOUCH_KEY_NONE; //������ֵ����
    }

    if(Matrix_Key_Value != MATRIX_KEY_NONE)
    {
        if((Matrix_Key_Value == MATRIX_KEY_0 || Matrix_Key_Value == MATRIX_KEY_1 || Matrix_Key_Value == MATRIX_KEY_2 || Matrix_Key_Value == MATRIX_KEY_3 || \
            Matrix_Key_Value == MATRIX_KEY_4 || Matrix_Key_Value == MATRIX_KEY_5 || Matrix_Key_Value == MATRIX_KEY_6 || Matrix_Key_Value == MATRIX_KEY_7 || \
            Matrix_Key_Value == MATRIX_KEY_8 || Matrix_Key_Value == MATRIX_KEY_9) && (is_manual_ing == true))
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
            offset = offset*10 + num;
            Lcd_printf("OFFSET:%d", offset);
            if(offset > 767)
            {
                Voice_Play((uint8_t)INPUT_ERROR);
                DiWenLcd_Printf("�������");  //������Χ��ʾ�������
                Lcd_printf("input error");
                offset = 0;
            }

            Matrix_Key_Value = MATRIX_KEY_NONE;
        }

        if(Matrix_Key_Value == MATRIX_KEY_D)
        {
            set_offset_finish = true;
            Y_OFFSET = offset;
            is_manual_ing = false;
            Lcd_printf("offset input success");

            Matrix_Key_Value = MATRIX_KEY_NONE;
        }

        if(Matrix_Key_Value == MATRIX_KEY_E)
        {
            offset = 0;
            is_manual_ing = true;
            Lcd_printf("input offset");
            Voice_Play((uint8_t)PLASE_INPUT_TARGET_Y);

            Matrix_Key_Value = MATRIX_KEY_NONE;
        }
    }

    if(vision_clib_flag == true)
    {
        static uint16_t num = 0;
        static uint8_t step = 0;
        static uint8_t point_i = 0;

        if(step == 0)
        {
            if(Vision_Dat_Explan() == 4)  //AIģ���⵽����
            {
                num++;
            }
            if(num >= 10)
            {
                num = 0;
                step = 1;
                Relay_Control(RELAY_ON);
            }  
        }
        
        if(step == 1)
        {
            if(num == 0)
            {
                Tripod_X_Coord(Vision_Coord.Res_Coord[point_i].x);
                Tripod_Y_Coord(Vision_Coord.Res_Coord[point_i].y); //������̨X��Y��ת��
                while(Tripod_Y.is_running);
                while(Tripod_X.is_running); //�ȴ�ת������
            }

            num++;

            if(num >= 2000)
            {
                num = 0;
                point_i++;
                if(point_i == 4)
                {
                    point_i = 0;
                    step = 0;
                    num = 0;
                    Relay_Control(RELAY_OFF);
                }
            }
        }
    }
}

void Logic_System_Check(void)
{
    static bool AI_receive_flag = false;
    uint8_t diwen_data;

    if(Touch_Key_Value != TOUCH_KEY_NONE)
    {
        switch (Touch_Key_Value)
        {
        case TOUCH_KEY_A:
            {
                Tripod_X_Angle(30);
                while(Tripod_X.is_running);
                HAL_Delay(100);
                Tripod_X_Angle(-30);
                while(Tripod_X.is_running);
                HAL_Delay(100);
                Tripod_X_Angle(0);
                while(Tripod_X.is_running);
                HAL_Delay(100);
                Tripod_Y_Angle(30);
                while(Tripod_Y.is_running);
                HAL_Delay(100);
                Tripod_Y_Angle(-30);
                while(Tripod_Y.is_running);
                HAL_Delay(100);
                Tripod_Y_Angle(0);
                while(Tripod_Y.is_running);
                HAL_Delay(100);
                Tripod_X_Go_Mechzero();
                Tripod_Y_Go_Mechzero();
            }
            break;

        case TOUCH_KEY_B:
            {
                static uint8_t num;
                num++;
                if(num > 33) num = 0;
                Voice_Play(num);
            }
            break;

        case TOUCH_KEY_C:
            {
                Infared_Send_OK();
                Infared.Infared_Is_Pause = false;
            }
            break;

        case TOUCH_KEY_D:
            {
                static bool num;
                num = !num;
                Relay_Control(num);
            }
            break;

        case TOUCH_KEY_LEFT:
            {
                select_shape++;
                if(select_shape>5)select_shape=1;
                select_color++;
                if(select_color>4)select_color=1;
                Vision_Send_Cmd(select_color, select_shape);
                AI_receive_flag = true;
                Lcd_printf("AI test start:%d %d",select_shape,select_color);
            }
            break;

        case TOUCH_KEY_RIGHT:
            {
                AI_receive_flag = false;
                Lcd_printf("AI test stop");
            }
            break;
        
        default:
            break;
        }
        Touch_Key_Value = TOUCH_KEY_NONE;
    }

    if(AI_receive_flag == true)
    {
        static uint16_t num;

        num++;
        if(num >= 1000)
        {
            num = 0;

            if(Vision_Dat_Explan() != 0)
            {
                uint8_t i;
                for(i=0; i<Vision_Coord.Res_Num; i++)
                {
                    Lcd_printf("corrd:%d, %d", Vision_Coord.Res_Coord[i].x, Vision_Coord.Res_Coord[i].y);
                }
            }
        }
    }

    diwen_data = DiWen_Dat_Explan();
    if(diwen_data == 1) Lcd_printf("DiWenLcd Up");
    else if(diwen_data == 2) Lcd_printf("DiWenLcd Down");

    if(Matrix_Key_Value != MATRIX_KEY_NONE)
    {
        switch(Matrix_Key_Value)
        {
            case MATRIX_KEY_0:
                Lcd_printf("key_0");
            break;
            case MATRIX_KEY_1:
                Lcd_printf("key_1");
            break;
            case MATRIX_KEY_2:
                Lcd_printf("key_2");
            break;
            case MATRIX_KEY_3:
                Lcd_printf("key_3");
            break;
            case MATRIX_KEY_4:
                Lcd_printf("key_4");
            break;
            case MATRIX_KEY_5:
                Lcd_printf("key_5");
            break;
            case MATRIX_KEY_6:
                Lcd_printf("key_6");
            break;
            case MATRIX_KEY_7:
                Lcd_printf("key_7");
            break;
            case MATRIX_KEY_8:
                Lcd_printf("key_8");
            break;
            case MATRIX_KEY_9:
                Lcd_printf("key_9");
            break;
            case MATRIX_KEY_F1:
                Lcd_printf("key_F1");
            break;
            case MATRIX_KEY_F2:
                Lcd_printf("key_F2");
            break;
            case MATRIX_KEY_F3:
                Lcd_printf("key_F3");
            break;
            case MATRIX_KEY_F4:
                Lcd_printf("key_F4");
            break;
            case MATRIX_KEY_D:
                Lcd_printf("key_D");
            break;
            case MATRIX_KEY_E:
                Lcd_printf("key_E");
            break;
            case MATRIX_KEY_UP:
                Lcd_printf("key_UP");
            break;
            case MATRIX_KEY_DOWN:
                Lcd_printf("key_DOWN");
            break;
            case MATRIX_KEY_LEFT:
                Lcd_printf("key_LEFT");
            break;
            case MATRIX_KEY_RIGHT:
                Lcd_printf("key_RIGHT");
            break;
        }

        Matrix_Key_Value = MATRIX_KEY_NONE;
    }
}



