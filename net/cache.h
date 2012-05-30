/********************************************
//�ļ���:cache.h
//����:��cache����
//����:�Ӻ���
//����ʱ��:2009.06.11
//�޸ļ�¼:

*********************************************/
#ifndef _COMM_SOCKCOMMU_CACHE_H_
#define _COMM_SOCKCOMMU_CACHE_H_

#include <sys/time.h>
#include "mempool.h"

namespace comm 
{
namespace sockcommu
{

//��cache����
class CRawCache
{
public:
    CRawCache(CMemPool& mp);
    ~CRawCache();

    char* data();
    unsigned data_len();
    void append(const char* data, unsigned data_len);
    void skip(unsigned length);

private:
    //�ڴ�ض���
    CMemPool& _mp;
    //�ڴ��ַ
    char* _mem;
    //�ڴ��С
    unsigned _block_size;
    //ʵ��ʹ���ڴ���ʼƫ����
    unsigned _data_head;
    //ʵ��ʹ���ڴ泤��
    unsigned _data_len;
};

//���Ӷ���cache
class ConnCache
{
public:
    ConnCache(CMemPool& mp) : _flow(0), _fd(0), _access(0), _type(0), _r(mp), _w(mp){}
    ~ConnCache(){}

    //����Ψһ��ʾ	
    unsigned _flow;
    //���fd
    int _fd;
    //ʱ���
    time_t _access;
    //��������: TCP_SOCKET\UDP_SOCKET\UNIX_SOCKET
    int _type;

    //������cache
    CRawCache _r;
    //д�ظ�cache
    CRawCache _w;
};
		
}
}
#endif

