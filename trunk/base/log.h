/********************************************
//�ļ���:log.h
//����:��־��
//����:�Ӻ���
//����ʱ��:2009.06.11
//�޸ļ�¼:

*********************************************/
#ifndef _COMM_LOG_LOG_H_
#define _COMM_LOG_LOG_H_
#include <time.h>
#include <stdarg.h>

/**********************************************************
�����̹߳���log�����ʱ��,��Ҫʹ��_THREAD_SAFE���뱾���
�������л��ļ���ʱ��������
***********************************************************/
#ifdef _THREAD_SAFE
#include <pthread.h>
#endif

/**********************************************************
������̻��߶��̷ֱ߳�ʹ�ö�����log��������ͬ������ʱҪʹ��_MP_MODE���뱾���
�������л��ļ���ʱ��������
***********************************************************/
#ifdef _MP_MODE
#include <sys/sem.h>
union logsemun
{
    int val;					//<= value for SETVAL
    struct semid_ds *buf;		//<= buffer for IPC_STAT & IPC_SET
    unsigned short int *array;	//<= array for GETALL & SETALL
    struct seminfo *__buf;		//<= buffer for IPC_INFO
};
#endif

#define DEFAULT_MAX_FILE_SIZE  (1 << 30)     //����ļ�SIZEΪ1G
#define DEFAULT_MAX_FILE_NO    1000          //Ĭ������ļ����
#define MAX_PATH_LEN           256           //���·������
#define MAX_LOG_LEN            4096          //һ����־��󳤶�
#define LOG_FLAG_TIME          0x01          //��ӡʱ���
#define LOG_FLAG_TID           0x02			 //��ӡ�߳�ID
#define LOG_FLAG_LEVEL         0x04			 //��ӡ��־����
#define ERR_FD_NO              0x00          //��ʼ�ļ�����ţ�Ĭ���Ǳ�׼���

namespace comm 
{
namespace log 
{

//��־����
enum LOG_TYPE
{
    LOG_TYPE_CYCLE = 0,
    LOG_TYPE_DAILY,
    LOG_TYPE_HOURLY,
    LOG_TYPE_CYCLE_DAILY,
    LOG_TYPE_CYCLE_HOURLY
};
//��־����
enum LOG_LEVEL
{
    LOG_TRACE = 0,
    LOG_DEBUG,
    LOG_NORMAL,
    LOG_ERROR,
    LOG_FATAL,
    LOG_NONE    //��Ҫ��ֹд�κ���־��ʱ��,��������tlog����־����ΪLOG_NONE
};

//���Ӻ���ԭ��
//fmt:		��ʽ�ַ���
//ap:       �ɱ��������
//����ֵ:   ����0��ʾ����¼����־�ļ�,�����¼����־�ļ�
typedef int (*log_hook) (const char *fmt, va_list ap);

//��־��
class CLog
{
public:
    CLog();
    ~CLog();

    //��ʼ����־
    //log_level:		��־����
    //log_type:			��־����
    //log_path:			��־���Ŀ¼
    //name_preifx:		��־�ļ���ǰ׺
    //max_file_size:	ÿ����־�ļ�����󳤶�
    //max_file_no:		��־�ļ�������
    //����ֵ:			0�ɹ�,����ʧ��
#ifndef _MP_MODE
    int log_open(int log_level, int log_type, const char* log_path, const char* name_prefix, int max_file_size, int max_file_no);
#else
    int log_open(int log_level, int log_type, const char* log_path, const char* name_prefix, int max_file_size, int max_file_no, int semkey /*�ź���key*/);
#endif
    //������־����
    //level:		�µ���־����
    //����ֵ:		�ϵ���־����
    int log_level(int level);
    //��ӡ��ʽ����־
    void log_i(int flag, int log_level, const char *fmt, ...);
    //��ӡbin��־
    void log_i_bin(int log_level, const char* buf, int len);
    //���ù��Ӻ���
    void log_set_hook(log_hook hook);
    //�Ѷ���������ת��Ϊ����ʾ��hex�ַ���
    //bin:		����������
    //len:      ���ݳ���
    //buf:      �ַ���������
    //����ֵ:   �ַ���ָ��
    static const char* to_hex(const char* bin, int len, char* buf);
    
protected:
    int log_level_;
    int log_type_;
    char log_path_[MAX_PATH_LEN];
    char name_prefix_[MAX_PATH_LEN];
    int max_file_size_;
    int max_file_no_;	

    int log_fd_;
    int cur_file_size_;
    int cur_file_no_;
    time_t pre_time_;
    char filename_[MAX_PATH_LEN];
    char buffer_[MAX_LOG_LEN];
    log_hook hook_;

    void close_file();
    void log_file_name(char* filepath, char* filename);
    int open_file();
    void init_cur_file_no();
    void force_rename(const char* src_pathname, const char* dst_pathname);
    int shift_file();

    void get_time(int& buff_len);
    void get_tid(int& buff_len);
    //void get_file(int& buff_len);
    void get_level(int& buff_len,int level);
#ifdef _THREAD_SAFE
    class CLogLock
    {
    public:
        CLogLock()
        {
            pthread_mutex_lock(&mutex_);
        }
        ~CLogLock()
        {
            pthread_mutex_unlock(&mutex_);
        }
    private:
        static pthread_mutex_t mutex_;	
    };
#endif
#ifdef _MP_MODE
    class CLogSemLock
    {
    public:
        CLogSemLock()
        {
            sem_lock();
        }
        ~CLogSemLock()
        {
            sem_unlock();
        }
        static int sem_init(int key);
    private:	
        int sem_lock();
        int sem_unlock();
        static int semid_;
    };
#endif								
};

/***********************************************
�û�ʹ��Ӧ�õ������¸�������Ӧ�ĺ�
************************************************/
#define LOG_OPEN					    log_open 
#define LOG_P(lvl, fmt, args...)  		    log_i(LOG_FLAG_TIME, lvl, fmt, ##args)
#define LOG_P_NOTIME(lvl, fmt, args...)    log_i(0, lvl, fmt, ##args)
#define LOG_P_PID(lvl, fmt, args...) 	    log_i(LOG_FLAG_TIME | LOG_FLAG_TID, lvl, fmt, ##args)
#define LOG_P_LEVEL(lvl, fmt, args...)	    log_i(LOG_FLAG_TIME | LOG_FLAG_LEVEL, lvl, fmt, ##args)
#define LOG_P_FILE(lvl, fmt, args...)	    log_i(LOG_FLAG_TIME, lvl, "[%-10s][%-4d][%-10s]"fmt, __FILE__, __LINE__, __FUNCTION__, ##args)
#define LOG_P_ALL(lvl, fmt, args...)		    log_i(LOG_FLAG_TIME | LOG_FLAG_LEVEL | LOG_FLAG_TID, lvl, "[%-10s][%-4d][%-10s]"fmt,__FILE__, __LINE__, __FUNCTION__, ##args)
#define LOG_P_BIN					    log_i_bin
#define SET_HOOK   				           log_set_hook

}
}

#endif 
