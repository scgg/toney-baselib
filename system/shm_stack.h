
/********************************
*�ļ�����shm_stack.h
*���ܣ����ڹ����ڴ��stack.
*���ߣ��ż���
*ʱ�䣺2009.06.10
**********************************/
#include "SVShm.hpp"
#include "HashMap.hpp"
#include "lock.hpp"
#include "lock.h"
#include "hexdump.h"

#ifndef _COMM_SHM_STACK_H_
#define _COMM_SHM_STACK_H_

using namespace comm::commu;
using namespace comm::lock;

namespace comm
{
namespace stack
{


template<typename data_t>
class CShmStack
{
public:

    typedef data_t node_t;

    typedef struct tagShmHead
    {
            size_t m_TotalItemNum;
            size_t m_CurItemNum;
            node_t m_Nodes[1];
    }ShmHead;


public:
 
    /*
    *����:��ʼ�������ڴ棬�ź���
    *����˵��: 
    *IN sem_key:�ź�����key
    *IN shm_key:�����ڴ��key
    *IN shm_size:�����ڴ�Ĵ�С
    *����ֵ: 0�ɹ�����0ʧ��
    */    
    int Init(int sem_key, int shm_key, size_t shm_size)
    {           
            int r = m_Lock.sem_init(sem_key);
            if(r != 0)
            {
                return -1;
            }

            int tmp_init = 0;
            comm::lock::CLockGuard g(&m_Lock);
            r = m_Shm.force_open_and_attach(shm_key, shm_size, tmp_init);
            if(r != 0)
            {
                return -2;
            }
            //tmp_init ��Ϊ0�������´�����shm.
            char *pShm = (char *)(m_Shm.get_segment_ptr());
  
            m_pHead = (ShmHead *)(pShm);
            if(tmp_init != 0)
            {
		//�����ڴ�Ϊ�´���

                m_pHead->m_TotalItemNum = (shm_size - sizeof(ShmHead))/sizeof(node_t)+1;
                //printf("#shm_size = %d, HeadLen=%d, nodesize=%d\n", shm_size, 
                     //sizeof(ShmHead),sizeof(node_t));
                m_pHead->m_CurItemNum = 0;
                //printf("#New Shm. Total = %d, cur=%d\n", m_pHead->m_TotalItemNum, m_pHead->m_CurItemNum);
            }
            else
            {
                //printf("Not Init:####Total = %d, cur=%d\n", m_pHead->m_TotalItemNum, m_pHead->m_CurItemNum);
            }
            return 0;
    }

    /*
    *����:ȡstack��Ԫ�صĸ���
    *����ֵ: stack��Ԫ�صĸ���
    */    
    size_t Size()
    {
        CLockGuard g(&m_Lock);
        return m_pHead->m_CurItemNum;
    }
    
    /*
    *����:stack�Ƿ�Ϊ��
    *����ֵ: true�գ�false����
    */    
    bool Empty()
    {
        return (Size() == 0);
    }

    /*
    *���ܣ���stack.
    *IN v:����stack��Ԫ��
    *����ֵ��0�ɹ���-1:stack��
    */
    int Push(const node_t &v)
    {
        CLockGuard g(&m_Lock);
        if(m_pHead->m_CurItemNum >= m_pHead->m_TotalItemNum)
        {
            return -1;
        }        
        m_pHead->m_Nodes[m_pHead->m_CurItemNum] = v;
        ++m_pHead->m_CurItemNum;
        return 0;
    }

    /*
    *���ܣ���stack.
    *OUT v:��stack��Ԫ��
    *����ֵ��0�ɹ���-1:stack��
    */
    int Pop(node_t &v)
    {
        CLockGuard g(&m_Lock);
        if(m_pHead->m_CurItemNum == 0)
        {
            return -1;
        }        
        v = m_pHead->m_Nodes[m_pHead->m_CurItemNum-1];
        --m_pHead->m_CurItemNum;
        return 0;
    }

private:
    CSVShm m_Shm;
    comm::lock::CSemLock m_Lock;
    
    ShmHead *m_pHead;
    node_t *m_pNodes;

};

}
}

#endif

