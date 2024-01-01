/**
 * @file      :_Debug.c
 * @brief     :ϵͳ��ʾģʽ����
 * @author    :shun (ljs@luntek.cn)
 * @version   :v1.0
 * @date      :2023-12-30
 * @attention :
 */
#include "_Debug.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

#include ".\tirpod\tirpod.h"

/****************************************************************
 *                                                              *
 *                                                              *
 *                      Debug Command                           *
 *                                                              *
 *                                                              *
 ***************************************************************/
/**
 * @brief :����������ָ��
 */
extern uint32_t g_ccr_val1; /* �Ƚ�ֵ��ֵ */
extern uint8_t g_run_flag1; /* ��־λ */
int angle1 = 0;

extern uint32_t g_ccr_val2; /* �Ƚ�ֵ��ֵ */
extern uint8_t g_run_flag2; /* ��־λ */
int angle2 = 0;

uint8_t *motor_ctl[] = {"ON", "OFF", "angle"};

int motorctl(int num, char *str)
{
    uint8_t command;
    for (int i = 0; i <= sizeof(motor_ctl); i++)
    {
        if (strcmp((char *)motor_ctl[i], str) == 0)
        {
            command = i;
            break;
        }
    }

    switch (num)
    {
    case STEPPER_MOTOR_1:
        switch (command)
        {
        case 0:
            stepper_star(STEPPER_MOTOR_1);
            printf("motor1 star\r\n");
            break;
        case 1:
            stepper_stop(STEPPER_MOTOR_1);
            printf("motor1 stop\r\n");
            break;
        case 2:
            if (g_run_flag1 == 0)
            {
                angle1 = 0;                                                           /* �Ƕ���0���Ա��´����� */
                stepper_set_angle(g_stepper1.angle, g_stepper1.dir, STEPPER_MOTOR_1); /* ������ת */
                printf("motor1 angle star\r\n");
            }
            break;
        case 3:
            break;
        }
        break;

    case STEPPER_MOTOR_2:
        if (strcmp(str, "ON") == 0)
        {
            stepper_star(STEPPER_MOTOR_2);
            printf("motor2 star\r\n");
        }
        else if (strcmp(str, "OFF") == 0)
        {
            stepper_stop(STEPPER_MOTOR_1);
            printf("motor2 stop\r\n");
        }
        break;
    }
    return 0;
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), motorctl, motorctl, "--motorctl (int)num [1 / 2] (char*)state [ ON / OFF]");

// ���1 �ٶ�ֵ���ã�speed1��
SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_VAR_INT), speed1, &g_ccr_val1, "motor1 speed ( 0~ 1000)");
// ���2 �ٶ�ֵ���ã�speed2��
SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_VAR_INT), speed2, &g_ccr_val2, "motor2 speed ( 0~ 1000)");

// ���1 ��ת�Ƕ����ã�angle1��
SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_VAR_INT), angle1, &angle1, "motor1 angle (-360 ~ 360 )");
// ���2 ��ת�Ƕ����ã�angle2��
SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_VAR_INT), angle2, &angle2, "motor2 angle (-360 ~ 360 )");

/****************************************************************/
/**
 * @brief     :ʾ��
 * @attention :
 */
// // main ������ʽ
// int mfunc(int argc, char *argv[])
// {
//     printf("%dparameter(s)\r\n", argc);
//     for (char i = 1; i < argc; i++)
//     {
//         printf("%s\r\n", argv[i]);
//     }
//     return 0;
// }
// SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_MAIN), mfunc, mfunc, main func);
// // ��ͨC������ʽ
// int nfunc(int i, char ch, char *str)
// {
//     printf("input int: %d, char: %c, string: %s\r\n", i, ch, str);
//     return 0;
// }
// SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), nfunc, nfunc, normal func);

// void log(void)
// {
//     printf("log test\r\n");
// }
// SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), log, log, elog func);

// // ����ʹ��
// int varInt = 0;
// SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_VAR_INT), varInt, &varInt, test);

// char str[] = "test string";
// SHELL_EXPORT_VAR(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_VAR_STRING), varStr, str, test);

Shell shell;
char shell_recv_buf;
char shellBuffer[512];

/**
 * @brief     :shellд���ݺ���ԭ��
 * @param     data :��д���ַ�����
 * @param     len :��Ҫд����ַ���
 * @return    :signed short ʵ��д����ַ�����
 * @attention :
 */
signed short userShellWrite(char *data, unsigned short len)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)data, len, 0xFFFF);
    return len;
}
/**
 * @brief     :shell��ȡ���ݺ���ԭ��
 * @param     data :shell��ȡ���ַ�
 * @param     len :�����ȡ���ַ�����
 * @return    :signed short ʵ�ʶ�ȡ�����ַ�����
 */
signed short userShellRead(char *data, unsigned short len)
{
    // HAL_UART_Receive(&huart1, (uint8_t *)data, len, 0xFFFF);
    return len;
}
/**
 * @brief     :���Խӿڳ�ʼ��
 * @attention :
 */
void _Debug_Shell_Init(void)
{
#ifdef __Debug
    shell.write = userShellWrite;
    shell.read = userShellRead;
    shellInit(&shell, shellBuffer, 512);
#endif
}
