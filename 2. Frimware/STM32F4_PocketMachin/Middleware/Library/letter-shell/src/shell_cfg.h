/**
 * @file shell_cfg.h
 * @author Letter (nevermindzzt@gmail.com)
 * @brief shell config
 * @version 3.0.0
 * @date 2019-12-31
 * 
 * @copyright (c) 2019 Letter
 * 
 */

#ifndef __SHELL_CFG_H__
#define __SHELL_CFG_H__


/**
 * @brief �Ƿ�ʹ��Ĭ��shell����whileѭ����ʹ�ܺ�`SHELL_USING_TASK`��˺�������
 *        ʹ�ܴ˺꣬��`shellTask()`������һֱѭ����ȡ���룬һ��ʹ�ò���ϵͳ����shell
 *        ����ʱʹ�ܴ˺꣬�رմ˺������£�һ���������޲���ϵͳ������ѭ���е���`shellTask()`
 */
#define     SHELL_TASK_WHILE            1

/**
 * @brief �Ƿ�ʹ���������ʽ
 *        ʹ�ܴ˺�󣬿���ʹ��`SHELL_EXPORT_CMD()`�ȵ�������
 *        ����shell����رմ˺������£���Ҫʹ�������ķ�ʽ
 */
#define     SHELL_USING_CMD_EXPORT      1

/**
 * @brief �Ƿ�ʹ��shell��������
 *        һЩ��չ�����(�ļ�ϵͳ֧�֣���־���ߵ�)��Ҫʹ�ð�������
 */
#define     SHELL_USING_COMPANION       0

/**
 * @brief ֧��shellβ��ģʽ
 */
#define     SHELL_SUPPORT_END_LINE      0

/**
 * @brief �Ƿ�����������б����г��û�
 */
#define     SHELL_HELP_LIST_USER        0

/**
 * @brief �Ƿ�����������б����г�����
 */
#define     SHELL_HELP_LIST_VAR         0

/**
 * @brief �Ƿ�����������б����г�����
 */
#define     SHELL_HELP_LIST_KEY         0

/**
 * @brief �Ƿ�����������б���չʾ����Ȩ��
 */
#define     SHELL_HELP_SHOW_PERMISSION  1

/**
 * @brief ʹ��LF��Ϊ�����лس�����
 *        ���Ժ�SHELL_ENTER_CRͬʱ����
 */
#define     SHELL_ENTER_LF              1

/**
 * @brief ʹ��CR��Ϊ�����лس�����
 *        ���Ժ�SHELL_ENTER_LFͬʱ����
 */
#define     SHELL_ENTER_CR              1

/**
 * @brief ʹ��CRLF��Ϊ�����лس�����
 *        �����Ժ�SHELL_ENTER_LF��SHELL_ENTER_CRͬʱ����
 */
#define     SHELL_ENTER_CRLF            0

/**
 * @brief ʹ��ִ��δ���������Ĺ���
 *        ���ú󣬿���ͨ��`exec [addr] [args]`ֱ��ִ�ж�Ӧ��ַ�ĺ���
 * @attention �����ַ���󣬿��ܻ�ֱ������������
 */
#define     SHELL_EXEC_UNDEF_FUNC       0

/**
 * @brief shell��������������
 *        �������������ڣ�����16����������ʹ���˲����Զ�ת��������£���Ҫ�޸�Դ��
 */
#define     SHELL_PARAMETER_MAX_NUMBER  8

/**
 * @brief ��ʷ�����¼����
 */
#define     SHELL_HISTORY_MAX_NUMBER    5

/**
 * @brief ˫�����(ms)
 *        ʹ�ܺ�`SHELL_LONG_HELP`��˺���Ч������˫��tab��ȫhelp��ʱ����
 */
#define     SHELL_DOUBLE_CLICK_TIME     200

/**
 * @brief ���ٰ���
 *        ������˫��tab�ĳ�������ʹ�ܴ˺�ʱ��˫��tab������������help��ȫ������ֱ����ʾ��Ӧ����İ�����Ϣ
 */
#define     SHELL_QUICK_HELP            1

/**
 * @brief ���������ֵ
 *        �������Ĭ�϶���һ��`RETVAL`�������ᱣ����һ������ִ�еķ���ֵ�����������������н��е���
 *        ��������`SHELL_CMD_DISABLE_RETURN`��־�����ã������������`RETVAL`
 */
#define     SHELL_KEEP_RETURN_VALUE     0

/**
 * @brief ��������shell����
 */
#define     SHELL_MAX_NUMBER            5

/**
 * @brief shell��ʽ������Ļ����С
 *        Ϊ0ʱ��ʹ��shell��ʽ�����
 */
#define     SHELL_PRINT_BUFFER          256

/**
 * @brief shell��ʽ������Ļ����С
 *        Ϊ0ʱ��ʹ��shell��ʽ������
 * @note shell��ʽ�����������shellTask, �����������в���ϵͳ�������ʹ��
 */
#define     SHELL_SCAN_BUFFER          0

/**
 * @brief ��ȡϵͳʱ��(ms)
 *        ����˺�Ϊ��ȡϵͳTick����`HAL_GetTick()`
 * @note �˺겻����ʱ�޷�ʹ��˫��tab��ȫ����help���޷�ʹ��shell��ʱ����
 */
#define     SHELL_GET_TICK()            0

/**
 * @brief ʹ����
 * @note ʹ��shell��ʱ����Ҫ�Լ����ͽ�������ʵ��
 */
#define     SHELL_USING_LOCK            0

/**
 * @brief shell�ڴ����
 *        shell������Ҫ�˽ӿڣ���ʹ��shell����������Ҫ���ж���
 */
#define     SHELL_MALLOC(size)          0

/**
 * @brief shell�ڴ��ͷ�
 *        shell������Ҫ�˽ӿڣ���ʹ��shell����������Ҫ���ж���
 */
#define     SHELL_FREE(obj)             0

/**
 * @brief �Ƿ���ʾshell��Ϣ
 */
#define     SHELL_SHOW_INFO             1

/**
 * @brief �Ƿ��ڵ�¼�����������
 */
#define     SHELL_CLS_WHEN_LOGIN        1

/**
 * @brief shellĬ���û�
 */
#define     SHELL_DEFAULT_USER          "Pocket-Mac"

/**
 * @brief shellĬ���û�����
 *        ��Ĭ���û�����Ҫ���룬��Ϊ""
 */
#define     SHELL_DEFAULT_USER_PASSWORD ""

/**
 * @brief shell�Զ�������ʱ
 *        shell��ǰ�û�������Ч��ʱ����Ч����ʱ����Զ���������shell
 *        ����Ϊ0ʱ�ر��Զ��������ܣ�ʱ�䵥λΪ`SHELL_GET_TICK()`��λ
 * @note ʹ�ó�ʱ�������뱣֤`SHELL_GET_TICK()`��Ч
 */
#define     SHELL_LOCK_TIMEOUT          0 * 60 * 1000

#endif
