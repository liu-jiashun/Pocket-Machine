#ifndef ___DEBUG_H__
#define ___DEBUG_H__

#include "main.h"
#include ".\letter-shell\src\shell.h"
#include ".\MALLOC\malloc.h"

#define __Debug // ����ģʽ�Ƿ�����ע�͸��У��رյ��Խӿڣ�

extern Shell shell;

extern char shell_recv_buf;

void _Debug_Shell_Init(void); // ���Խӿڳ�ʼ��

#endif
