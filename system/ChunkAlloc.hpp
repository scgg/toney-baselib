#ifndef _CHUNK_ALLOC_HPP_
#define _CHUNK_ALLOC_HPP_

#include "error.hpp"
#include <string.h>
#include <assert.h>
#include <time.h>


namespace comm
{
namespace commu
{

//Chunk Handler�ڴ�ƫ����
//typedef int BC_MEM_HANDLER;
typedef long long  BC_MEM_HANDLER;
const BC_MEM_HANDLER INVALID_BC_MEM_HANDLER = -1;


#ifndef NULL
#define NULL 0
#endif


//Ԥ�����ڴ��������ݴ洢��Ƭ��

#pragma pack(1)

typedef struct tagTChunkNode
{
    BC_MEM_HANDLER next_;     //CHUNK��Ƭ��ָ��
    char data_[1];              //������
}TChunkNode;

typedef struct tagTChunk
{
    int chunk_total_;           //CHUNK�ܽڵ���
    int chunk_size_;            //CHUNK����Ƭ�ߴ�
    int free_total_;            //����CHUNK����
    BC_MEM_HANDLER free_list_;    //����CHUNK����ͷָ��
    TChunkNode chunk_nodes_[1]; //CHUNK����
}TChunk;

#pragma pack()


class CChunkAllocator 
{
public:
    enum CHUNK_ALLOCATOR_ERROR
    {
        CHUNK_ALLOCATOR_ERROR_BASE = -1,    
        CHUNK_ALLOCATOR_ERROR_INVALID_PARAM = CHUNK_ALLOCATOR_ERROR_BASE -1,    //�Ƿ�����
        CHUNK_ALLOCATOR_ERROR_FREE_CHUNK_LACK = CHUNK_ALLOCATOR_ERROR_BASE -2,    //�����ڴ�鲻��
        CHUNK_ALLOCATOR_ERROR_DATA_VERIFY_FAIL = CHUNK_ALLOCATOR_ERROR_BASE -3,    //�ڴ����ݼ��ʧ��
    };
    
public:
    //��ʼ��CHUNK �ڴ��    
    int open(char *pool,bool init, int n_chunks, int chunk_size);
    
    //�ӿ��������з���CHUNK.  
    BC_MEM_HANDLER malloc (int chunk_num=1);
    
    //��CHUNK���뵽����������.
    void free(BC_MEM_HANDLER head_chunk_hdr);

    //��CHUNK�е����ݷ�Ƭ�������
    int merge(BC_MEM_HANDLER chunk_node_hdr, int chunk_len,
                void* data_buf, int* data_len);
        
    //�����ݷ�Ƭ���ڵ���CHUNK��.
    void split(BC_MEM_HANDLER head_hdr, const void* data_buf, int data_len);        

    //��ƫ����ת������ʵ���ڴ��ַ
    TChunkNode *handler2ptr(BC_MEM_HANDLER handler);
    
    //���ڴ��ַת����ƫ����
    BC_MEM_HANDLER ptr2handler(TChunkNode* ptr);
   
    //������Ҫ����CHUNK�������ݴ洢. 
    int get_chunk_num(int data_len);    

    //����ʹ�õ�CHUNK��
    int get_used_chunk_num() { return (chunk_->chunk_total_ - chunk_->free_total_); };
    
    //���ؿ��е�CHUNK��
    int get_free_chunk_num() { return chunk_->free_total_; };

    //����CHUNK ���ڴ��ߴ�
    static size_t get_chunk_size(int n_chunks, int chunk_size)
    {
        return (sizeof(TChunkNode) - sizeof(char[1]) + chunk_size); 
    }
    static size_t get_pool_size(int n_chunks, int chunk_size)
    {
        size_t chunk_total_size = n_chunks * get_chunk_size(n_chunks, chunk_size);
        size_t head_size = (sizeof(TChunk) - sizeof(TChunkNode));
        size_t pool_size = head_size + chunk_total_size;
        return pool_size;
    }

protected:    
    //��CHUNK���뵽����������.
    void free_list_insert(TChunkNode *node);

    //�ӿ��������з���CHUNK.  
    TChunkNode *free_list_remove();

    //��ʼ��CHUNK �ڴ���е����ݽṹ
    void init_pool_data(int n_chunks, int chunk_size);

    //��� CHUNK �ڴ���е����ݽṹ
    int verify_pool_data(int n_chunks, int chunk_size);

    char *pool_;        //CHUNK �ڴ����ʼ��ַ
    char *pool_tail_;   //CHUNK �ڴ�������ַ
    TChunk *chunk_; //�ڴ���е� TChunk �ṹ��ָ��
};

}
}


#endif 
