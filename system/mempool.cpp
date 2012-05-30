

#include <cassert>
#include <iostream>
#include "mempool.h"

using namespace std;

namespace comm
{
namespace sockcommu
{

static const unsigned int C_EXTEND_SET = 128;
static const unsigned int C_SLOT_1 = 1<<20;     // 1M
static const unsigned int C_SLOT_2 = 1<<26;     // 64M
static const unsigned int C_ALLOC_CAPCITY= 2*C_SLOT_2;    // 128M 

//////////////////////////////////////////////////////////////////////////

CMemPool::~CMemPool()
{
    mms::iterator it;
    for(it = _stub.begin(); it != _stub.end(); ++it)
    {
        set<void*>* p_set = it->second;
        set<void*>::iterator iit;
        for(iit = p_set->begin(); iit != p_set->end(); ++iit)
        {
            delete [] (char*)*iit;    
        }
    }
}
void* CMemPool::allocate(unsigned int size, unsigned int & result_size)
{
    //	first, get block size
    unsigned int block_size = fit_block_size(size);

    //	second, find the size pool
    mml::iterator it_list = _free.find(block_size);

    //	third, if size empty, extend for a list and a stub
    if (it_list == _free.end())
    {
        if (extend_new_size(block_size)!=0)
        {
            cerr << __FILE__ << __LINE__ << "alloc_size" << _allocated_size << endl;
            return NULL;
        }

        it_list = _free.find(block_size);
        if (it_list == _free.end())
        {
            cerr << __FILE__ << __LINE__ << "alloc_size" << _allocated_size << endl;
            return NULL;
        }
    }
    
    //	fourth, get from pool	
    list<void*>* p_list = it_list->second;
    list<void*>::iterator it_block = p_list->begin();
    if (it_block == p_list->end())
    {
        mms::iterator it_set = _stub.find(block_size);

        if (extend(block_size, p_list, it_set->second) != 0)
        {
            cerr << __FILE__ << __LINE__ << "alloc_size" << _allocated_size << endl;
            return NULL;
        }

        it_block = p_list->begin();
        if (it_block == p_list->end())
        {
            cerr << __FILE__ << __LINE__ << "alloc_size" << _allocated_size << endl;
            return NULL;
        }
    }

    void* ret = *it_block;
    p_list->pop_front();
    result_size = block_size;

    //	return
    return ret;
}


int CMemPool::recycle(void* mem, unsigned int mem_size)
{
    //	first, check block size
    //unsigned block_size = fit_block_size(mem_size);
    //if (mem_size != block_size)
    //	return -1;

    //	second, sure the mem in stub
    mms::iterator it_set = _stub.find(mem_size);
    if (it_set == _stub.end())
        return -1;

    set<void*>* p_set = it_set->second;
    set<void*>::iterator it_mem = p_set->find(mem);
    if (it_mem == p_set->end())
        return -1;

    //	third, add into free list
    mml::iterator it_list = _free.find(mem_size);
    if (it_list == _free.end())
        return -1;

    list<void*>* p_list = it_list->second;
    p_list->push_front(mem);

    release(mem_size, p_list, p_set);
    //	return
    return 0;
}


int CMemPool::release_size(unsigned int mem_size)
{
    // ˮ��λ��Ϊ�����ݻ�
    _water_mark = C_ALLOC_CAPCITY;

    unsigned int purge_size = 1 << 31;
    unsigned int last_alloc_size = _allocated_size;

    while(((last_alloc_size - _allocated_size) < mem_size)
        && (purge_size>=C_SLOT_1))
    {
        purge_size = purge_size >> 1;
        mms::iterator it_set = _stub.find(purge_size);
        if (it_set == _stub.end())
        continue;

        set<void*>* p_set = it_set->second;

        mml::iterator it_list = _free.find(purge_size);
        if (it_list == _free.end())
            continue;

        list<void*>* p_list = it_list->second;

        release(purge_size, p_list, p_set);
    }

    // ˮ��λ�ڴ�ص����ˮ��λ
    _water_mark = C_MAX_WATER_MARK;
    if ((last_alloc_size - _allocated_size) < mem_size)
    {
        return -1;
    }

    //	return
    return 0;

}

inline unsigned CMemPool::fit_extend_set(unsigned int size)
{
    if(size < C_SLOT_1)
    {
        return C_EXTEND_SET;
    }
    else if((size >= C_SLOT_1) && (size < C_SLOT_2))
    {
        // >=C_SLOT_1 && <C_SLOT_2, ʱ���ܷ�������ΪC_ALLOC_CAPCITY
        return C_ALLOC_CAPCITY/size;
    }
    else	//(size >= C_SLOT_2)
    {
        // > C_SLOT_2,����һ��
        return 1;
    }
}

int CMemPool::extend(unsigned int size, std::list<void*>* l, std::set<void*>* s)
{
    unsigned int extend_set = fit_extend_set(size);    

    if (_allocated_size + size * extend_set >= C_MAX_POOL_SIZE)
    {
        if (release_size(_allocated_size + size * extend_set - C_MAX_POOL_SIZE ) != 0)
        {
            return -1;
        }
    }

    //	extend it
    for(unsigned int i = 0; i < extend_set; i++)
    {
        void* p = new char[size];
        l->push_front(p);
        s->insert(p);
    }
    
    _allocated_size += size * extend_set;

    //	fourth, return
    return 0;
}

int CMemPool::extend_new_size(unsigned int size)
{
    list<void*>* l = new list<void*>;
    set<void*>* s = new set<void*>;

    _free.insert(mml::value_type(size, l));
    _stub.insert(mms::value_type(size, s));

    //	fourth, return
    return extend(size, l, s);
}

int CMemPool::release(unsigned int size,  list<void*>* l, set<void*>* s)
{
    //	release it
    unsigned int todo_count = release_size(size, l->size(), s->size());
    if (todo_count == 0)
        return 0;

    assert(_allocated_size >= size * todo_count);

    //	assert(todo_count < l->size());
    for(unsigned int i = 0; i < todo_count; i++)
    {
        void* p = l->front();
        char* cp = (char*) p;
        delete [] cp;
        l->pop_front();
    }

    _allocated_size -= size * todo_count;

    //	fourth, return
    return 0;
}

//
//	rule one, free 1/2, release 1/4
//	rule two, free 1024. release 512
//	rule three, free < 1M
//
unsigned CMemPool::release_size(unsigned int block_size, unsigned int free_count, unsigned int stub_count)
{
    if (free_count * block_size < (1<<20))
        return 0;

    if (block_size > C_SLOT_2)
    {
        // >C_SLOT_2
        // rule one free >1 ,release 1/2
        // rule two free =1, release 1
        return (free_count>1)?(free_count/2):free_count;
    }

    // ���пռ����ˮ��λ����ʼ�ͷ�
    if (free_count >= stub_count / 2 || free_count*block_size >= _water_mark)
        return (free_count/2);

    return 0;
}

}
}


