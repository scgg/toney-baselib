
#ifndef _REPLY_HASH_MAP_
#define  _REPLY_HASH_MAP_

#include "ChunkAlloc.hpp"
#include <ext/hash_map>
#include <functional>
//using namespace __gnu_cxx;      // hash map is int __gnu_cxx namespace

namespace comm
{
namespace commu
{


typedef enum tagENodeFlag
{
    NODE_FLAG_UNCHG = 0x00,
    NODE_FLAG_DIRTY = 0x01,
}ENodeFlag;


typedef struct tagTHashPara
{
    int bucket_size_; //HASHͰ��
    int node_total_; //�ڵ���
    int chunk_total_; //CHUNK��Ƭ��
    int chunk_size_; //CHUNK��Ƭ��С
}THashPara; 

typedef struct tagTHashInfo
{
    THashPara hash_para_;
    int shm_key_; //shm key
    int sem_key_; //sem key
}THashInfo; 


#pragma pack(1)

template<typename key_t>
struct HashNode
{
    key_t   key_;                       //����
    int chunk_len_;                 //CHUNK�е����ݳ���
    BC_MEM_HANDLER chunk_head_;   //CHUNK ���
    BC_MEM_HANDLER node_prev_;    //�ڵ�����ǰָ��
    BC_MEM_HANDLER node_next_;    //�ڵ������ָ��
    BC_MEM_HANDLER add_prev_;     //��������ǰָ��
    BC_MEM_HANDLER add_next_;     //���������ָ��
    int add_info_1_;                //������ʱ��
    int add_info_2_;		//���ʴ���
    int flag_;//�ڵ���
};



typedef struct tagTHashMap
{	
    int node_total_;                //�ڵ�����
    int bucket_size_;               //HASHͰ�Ĵ�С
    int used_node_num_;             //ʹ�õĽڵ���
    int used_bucket_num_;           //HASHͰʹ����
    BC_MEM_HANDLER add_head_;     //��������ͷָ��
    BC_MEM_HANDLER add_tail_;     //��������βָ��
    BC_MEM_HANDLER free_list_;    //�ռ�ڵ�����ͷָ��
    BC_MEM_HANDLER bucket[1];     //HASHͰ
}THashMap;

#pragma pack()

template<
    typename key_t ,
    typename node_t = HashNode<key_t>,
    typename HashFunc = __gnu_cxx::hash<key_t>
    > 
class CHashMap
{
public:
    enum HASH_MAP_ERROR
    {
        HASH_MAP_ERROR_BASE = -1000,    
        HASH_MAP_ERROR_INVALID_PARAM = HASH_MAP_ERROR_BASE -1,    //�Ƿ�����
        HASH_MAP_ERROR_NODE_NOT_EXIST = HASH_MAP_ERROR_BASE -2,    //�ڵ㲻����
        HASH_MAP_ERROR_NODE_HAVE_EXIST = HASH_MAP_ERROR_BASE -3,    //�ڵ��Ѿ�����
        HASH_MAP_ERROR_NO_FREE_NODE = HASH_MAP_ERROR_BASE -4,    //û�п��нڵ�
    };

public:
    CHashMap();
    ~CHashMap();    

    //��ʼ�� HASH_MAP �ڴ��
    int open(char* pool, bool init, int node_total, int bucket_size, int n_chunks, int chunk_size);

    // ʹ�� <key> ���в�ѯ.
    node_t * find_node(const key_t &key);    
     //����ڵ�, ����ɽڵ����, �򷵻�ʧ��
    node_t * insert_node(const key_t &key, void* new_data, int new_len);
    //�޸Ľڵ�
    node_t * update_node(node_t * node, void* new_data, int new_len, 
    								char* old_data = NULL, int* old_len = NULL);
    //insert or update
    node_t * replace_node(const key_t &key, void* new_data, int new_len, char* old_data = NULL, int* old_len = NULL);
    //ɾ�����. ͬʱ�Ὣ�ڵ�Ӹ������������
    //����ֵ = 0 ��ʾ�ɹ�, < 0 ��ʾʧ��(��ڵ㲻����,Ҳ����ʧ��)
    int delete_node(node_t * node, char* data = NULL, int* data_len = NULL);

    int merge_node_data(node_t * node, char* data, int* data_len);

    // ���ص�ǰ�ڵ�ʹ����
    int used_node_num() { return hash_map_->used_node_num_; }
    int free_node_num() { return hash_map_->node_total_ - hash_map_->used_node_num_; }
    int get_node_total() { return hash_map_->node_total_; }
    int get_bucket_used() { return hash_map_->used_bucket_num_; }
    int free_bucket_num() {return hash_map_->bucket_size_ - hash_map_->used_bucket_num_; }
    int get_bucket_size() {return hash_map_->bucket_size_;}

    CChunkAllocator* chunks() {return &allocator_; }

    // ����HASH_MAP��������ڴ��ߴ�
    static size_t get_pool_size(int node_total, int bucket_size)
    {
        size_t head_size = sizeof(THashMap) - sizeof(BC_MEM_HANDLER[1]);
        size_t bucket_total_size = bucket_size * sizeof(BC_MEM_HANDLER);
        size_t node_total_size = node_total * sizeof(node_t);

        size_t pool_size = head_size + bucket_total_size + node_total_size;
        return pool_size; 
    }
    // ȡHASH_MAP ��CHUNK���ڴ��ߴ�
    static size_t get_total_pool_size(int node_total, int bucket_size, int n_chunks, int chunk_size)
    {
        return get_pool_size(node_total, bucket_size) + CChunkAllocator::get_pool_size(n_chunks, chunk_size);
    }

    //transform handler to address
    node_t *handler2ptr(BC_MEM_HANDLER handler);

    //transform address to handler
    BC_MEM_HANDLER ptr2handler(node_t * ptr);

    //���������������
    void insert_add_list_head(node_t * node);
    void insert_add_list_tail(node_t * node);
    void delete_from_add_list(node_t * node);
    node_t * get_add_list_prev(node_t * node);
    node_t * get_add_list_next(node_t * node);
    node_t * get_add_list_head();
    node_t * get_add_list_tail();
    ////////////////	

    void set_node_flag(node_t  * node, ENodeFlag f){assert(node); node->flag_ = (int)f;}
    ENodeFlag get_node_flag(node_t  *node){assert(node); return (ENodeFlag)node->flag_;}
    node_t * get_bucket_list_head(unsigned bucket_id);
    node_t * get_bucket_list_prev(node_t * node);
    node_t * get_bucket_list_next(node_t * node);

protected:

    void init_pool_data(int node_total, int bucket_size);
    int verify_pool_data(int node_total, int bucket_size);

    //������������HASHͰֵ
    int get_bucket_id(const key_t &key);
    int get_bucket_list_len(int bucket_id); //ȡHASHͰ����ײ��

    //���ڵ���뵽��������
    void free_list_insert(node_t  *node);
    //�ӿ���������ȡ�ڵ�
    node_t  *free_list_remove();

    //�ڵ������������
    void insert_node_list(node_t * node);
    void delete_from_node_list(node_t * node);

    //��ʼ���ڵ�
    void init_node(node_t * node);
    //���ڵ���Ϊ����ģʽ
    void free_node(node_t  *node);
    //���ڵ���Ϊʹ��ģʽ
    void use_node(node_t  *node,const key_t &key, int chunk_len, BC_MEM_HANDLER chunk_head);

    char *pool_;        //�ڴ����ʼ��ַ
    char *pool_tail_;   //�ڴ�������ַ

    HashFunc hash_func_;//HASH����
    THashMap* hash_map_;   //�ڴ���е�HASHMAP �ṹ
    node_t * hash_node_; //�ڴ���е�HASH�ڵ�����
    CChunkAllocator allocator_; //CHUNK������

};

}
}


#include "HashMapImp.hpp"

#endif

