#include <stdlib.h>
#include <string.h>
#include "mem_manager.h"


void init_mem_store(struct mem_store* p)
{
    p->mem_volume_dose = p->current_usage_amount = p->surplus_volume_dose = p->start_address = p->end_address = 0;
}

void init_mem_list(struct mem_list* p)
{
    p->start_address = p->end_address = p->current_usage_amount = 0;
    p->bvaild = 0;
    p->priv = p->next = 0;
}

void init_mempool(struct mempool* p)
{
    p->m_memory = p->m_memhead = p->m_memend = 0;
    p->m_ncomparemax = 5;
    p->m_ncomparemini = 1;
    p->m_memory->pinit = init_mem_store;
    p->m_memhead->pinit = init_mem_list;
    p->pmem_init = mem_init;
    p->pmem_alloc = mem_alloc;
    p->pmem_free = mem_free;
    p->pmem_destroy = mem_destroy;
}

bool mem_init(struct mempool* p, void *mem_start_addr, int mem_size)
{
    if (p->m_memory == 0)
    {
        p->m_memory = (pmem_store)malloc(sizeof(pmem_store));
        p->m_memory->pinit(&(p->m_memory));
    }
    if (p->m_memory == 0)
    {
        return false;	//表示内存池分配出错
    }
    if (mem_size > 0)
    {
        p->m_memory->mem_volume_dose = mem_size;
        p->m_memory->start_address = (unsigned long long)mem_start_addr;
        p->m_memory->end_address = p->m_memory->start_address + mem_size;
    }
    else
    {
        p->m_memory->mem_volume_dose = MEMPOOLSIZE;
        p->m_memory->start_address = (unsigned long long)mem_start_addr;
        p->m_memory->end_address = p->m_memory->start_address + MEMPOOLSIZE;
    }
    p->m_memory->current_usage_amount = 0;
    p->m_memory->surplus_volume_dose = p->m_memory->mem_volume_dose;

    if (p->m_memory->start_address == 0)
    {
        return false;	//表示内存池分配出错
    }
    return true;
}

void *mem_alloc(struct mempool* p, int size)
{
    if (p->m_memhead == 0)
    {
	//若没创建头节点，则创建头节点
        if (p->m_memory == 0)
        {
            return 0;	//没创建内存池的化需先创建内存池，否则返回0
        }
        p->m_memhead = (pmem_list)malloc(sizeof(pmem_list));
        p->m_memhead->pinit(&(p->m_memhead));
        p->m_memhead->start_address = (p->m_memory->start_address + p->m_memory->current_usage_amount);
	//多分配一个字节，防止内存越界
        p->m_memhead->end_address = (p->m_memhead->start_address + size + 1);
        p->m_memhead->current_usage_amount = (size + 1);
        p->m_memhead->bvaild = 1;
        p->m_memory->current_usage_amount += (size + 1);
        p->m_memory->surplus_volume_dose -= p->m_memory->current_usage_amount;
        p->m_memend = p->m_memhead;
	//分配出一段干净的内存，方便上层使用
        memset((void *)p->m_memhead->start_address, 0, size + 1);
        return (void *)p->m_memhead->start_address;
    }
    else
    {
	//判断内存池容量是否足够
        if (p->m_memory->surplus_volume_dose <= 0 || p->m_memory->surplus_volume_dose <= size)
        {
            return 0;
        }
        else
        {
            pmem_list listdata = p->m_memhead;
            while (listdata && listdata->next != 0)
            {
		//遍历是否有满足要求的解除占用的节点，没找到则在末端添加节点
                if (((size + p->m_ncomparemini <= listdata->current_usage_amount) && (size + p->m_ncomparemax >= listdata->current_usage_amount)) && (0 == listdata->bvaild))
                {
                    p->m_memory->current_usage_amount += listdata->current_usage_amount;
                    p->m_memory->surplus_volume_dose = p->m_memory->mem_volume_dose - p->m_memory->current_usage_amount;
                    listdata->bvaild = 1;
                    return (void *)listdata->start_address;
                }
                listdata = listdata->next;
            }

            listdata = p->m_memend;
            listdata->next = (pmem_list)malloc(sizeof(pmem_list));
            listdata->next->pinit = init_mem_list;
            listdata->next->pinit(&(listdata->next));
            listdata->next->start_address = (p->m_memend->end_address + 1);

            listdata->next->end_address = (listdata->next->start_address + size + 1);
            listdata->next->current_usage_amount = (size + 1);
            listdata->next->priv = listdata;
            listdata->next->bvaild = 1;
            p->m_memory->current_usage_amount += (size + 1);
            p->m_memory->surplus_volume_dose = (p->m_memory->mem_volume_dose - p->m_memory->current_usage_amount);
            p->m_memend = listdata->next;
            memset((void *)listdata->next->start_address, 0, size + 1);
            return (void *)listdata->next->start_address;
        }
    }
}

void mem_free(struct mempool* p, void *mem_addr)
{
    pmem_list listdata = p->m_memhead;
    while (listdata)
    {

        if (listdata->start_address == (unsigned long)mem_addr && (listdata->bvaild == 1))
        {
            p->m_memory->current_usage_amount -= listdata->current_usage_amount;
            p->m_memory->surplus_volume_dose = p->m_memory->mem_volume_dose - p->m_memory->current_usage_amount;
            if ((listdata->priv->bvaild != 0) || (listdata->priv == 0))	//判断是否为头节点或前一节点是否也解除占用
            {
                listdata->bvaild = 0;
            }
            else
            {
                pmem_list priv = 0, next = 0;
                priv = listdata->priv;
                if (listdata->next != 0)
                {
                    next = listdata->next;
                }
                priv->current_usage_amount += listdata->current_usage_amount;
                priv->end_address = priv->start_address + priv->current_usage_amount;

                free(listdata);
                priv->next = next;
                if (next != 0)
                {
                    next->priv = priv;
                }
                else
                {
                    p->m_memend = priv;
                }

            }
            return;
        }
        listdata = listdata->next;
    }
}

void mem_destroy(struct mempool* p, void *mem_start_addr)
{
    if (p->m_memory == 0)
    {
        return;
    }
    else
    {
        pmem_list listdata = p->m_memhead;
        while (listdata)
        {
            listdata->start_address = listdata->end_address = listdata->current_usage_amount = 0;
            listdata->bvaild = 0;
            listdata = listdata->next;
        }
        free(p->m_memory);
        listdata = p->m_memhead;
        while (listdata)
        {
            pmem_list list = listdata;
            pmem_list next = list->next;
            free(list);
            if (next != 0)
            {
                    next->priv = 0;
            }
            else
            {
                    p->m_memend = p->m_memhead = 0;
            }
            listdata = next;
        }
    }
}

