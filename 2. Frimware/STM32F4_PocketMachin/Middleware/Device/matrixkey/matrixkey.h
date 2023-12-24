#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "main.h"
#include ".\FlexibleButton\flexible_button.h"

typedef enum // 矩阵按键键值枚举
{
    MATRIX_KEY_NONE = 0, // 无按键按下
    MATRIX_KEY_UP,       // 按键“上”
    MATRIX_KEY_7,        // 按键“7”
    MATRIX_KEY_8,        // 按键“8”
    MATRIX_KEY_9,        // 按键“9”
    MATRIX_KEY_DOWN,     // 按键“下”
    MATRIX_KEY_4,        // 按键“4”
    MATRIX_KEY_5,        // 按键“5”
    MATRIX_KEY_6,        // 按键“6”
    MATRIX_KEY_LEFT,     // 按键“左”
    MATRIX_KEY_1,        // 按键“1”
    MATRIX_KEY_2,        // 按键“2”
    MATRIX_KEY_3,        // 按键“3”
    MATRIX_KEY_RIGHT,    // 按键“右”
    MATRIX_KEY_D,        // 按键“D”
    MATRIX_KEY_0,        // 按键“0”
    MATRIX_KEY_E,        // 按键“E”
    MATRIX_KEY_F1,       // 按键“F1”
    MATRIX_KEY_F2,       // 按键“F2”
    MATRIX_KEY_F3,       // 按键“F3”
    MATRIX_KEY_F4,       // 按键“F4”

    MATRIX_BUTTON_MAX,
} matrix_button_t;

void matrixkey_init(void);

static uint8_t matrix_btn_read(void *arg);
static void matrix_btn_evt_cb(void *arg);
static void matrix_combination_btn_event(flex_button_t *btn);

uint8_t keyboard_scan(void);

#endif
