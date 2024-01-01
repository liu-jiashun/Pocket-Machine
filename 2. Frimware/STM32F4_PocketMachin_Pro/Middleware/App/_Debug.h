#ifndef ___DEBUG_H__
#define ___DEBUG_H__

#include "main.h"
#include ".\letter-shell\src\shell.h"
#include ".\MALLOC\malloc.h"

#define __Debug // 调试模式是否开启（注释该行，关闭调试接口）

extern Shell shell;

extern char shell_recv_buf;

void _Debug_Shell_Init(void); // 调试接口初始化

#endif
