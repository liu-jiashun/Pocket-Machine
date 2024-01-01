#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "main.h"
#include ".\FlexibleButton\flexible_button.h"

typedef enum // ���󰴼���ֵö��
{
    MATRIX_KEY_NONE = 0, // �ް�������
    MATRIX_KEY_UP,       // �������ϡ�
    MATRIX_KEY_7,        // ������7��
    MATRIX_KEY_8,        // ������8��
    MATRIX_KEY_9,        // ������9��
    MATRIX_KEY_DOWN,     // �������¡�
    MATRIX_KEY_4,        // ������4��
    MATRIX_KEY_5,        // ������5��
    MATRIX_KEY_6,        // ������6��
    MATRIX_KEY_LEFT,     // ��������
    MATRIX_KEY_1,        // ������1��
    MATRIX_KEY_2,        // ������2��
    MATRIX_KEY_3,        // ������3��
    MATRIX_KEY_RIGHT,    // �������ҡ�
    MATRIX_KEY_D,        // ������D��
    MATRIX_KEY_0,        // ������0��
    MATRIX_KEY_E,        // ������E��
    MATRIX_KEY_F1,       // ������F1��
    MATRIX_KEY_F2,       // ������F2��
    MATRIX_KEY_F3,       // ������F3��
    MATRIX_KEY_F4,       // ������F4��

    MATRIX_BUTTON_MAX,
} matrix_button_t;

void matrixkey_init(void);

static uint8_t matrix_btn_read(void *arg);
static void matrix_btn_evt_cb(void *arg);
static void matrix_combination_btn_event(flex_button_t *btn);

uint8_t keyboard_scan(void);

#endif
