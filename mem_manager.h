#include <stdio.h>
#include <stdbool.h>
#define MEMPOOLSIZE 100*1024*1024

typedef struct mem_store
{
    unsigned long long mem_volume_dose;		//内存池总量
    unsigned long long current_usage_amount;	//当前使用量
    unsigned long long surplus_volume_dose;	//剩余量
    unsigned long long start_address;		//起始地址
    unsigned long long end_address;		//结束地址
    void (*pinit)(struct mem_store *);

}* pmem_store;

typedef struct mem_list
{
    unsigned long start_address;	//起始地址
    unsigned long end_address;		//结束地址
    unsigned long current_usage_amount;	//链表节点当前使用量
    bool bvaild;			//表示是否被占用
    struct mem_list *priv, *next;
    void (*pinit)(struct mem_list *);

}* pmem_list;


struct mempool
{
    pmem_store m_memory;		//内存池对象
    pmem_list m_memhead, m_memend;	//链表节点对象
    int m_ncomparemini;			//匹配最小值
    int m_ncomparemax;			//匹配最大值
    void (*pinit)(struct mempool*);
    bool (*pmem_init)(struct mempool*, void *, int);
    void* (*pmem_alloc)(struct mempool*, int);
    void (*pmem_free)(struct mempool*, void *);
    void (*pmem_destroy)(struct mempool*, void *);
};

void init_mem_store(struct mem_store *);
void init_mem_list(struct mem_list *);
void init_mempool(struct mempool*);
bool mem_init(struct mempool*, void *, int );
void *mem_alloc(struct mempool*, int);
void mem_free(struct mempool*, void *);
void mem_destroy(struct mempool*, void *);
