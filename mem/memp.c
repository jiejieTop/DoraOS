#include <memp.h>
#include <mem.h>
#include <log.h>
#include <dos_config.h>

/**
 * create a memory pool
 * @param   base:       memory pool base addr
 * @param   memp_size:  memory pool size
 * @param   blk_size:   memory block size
 */
Dos_Memp_t Dos_MempCreate(
#if (DOS_HEAP_TO_MEMP == 0U)
    dos_void *base,
#endif 
    dos_uint32 memp_size,
    dos_uint32 blk_size)
{
    dos_uint32 i,memp_cnt;
    Dos_Memp_t memp;
    Dos_MempNode_t node;
    Dos_MempNode_t next_node;

    if((blk_size == 0) || (memp_size == 0) || (memp_size < blk_size))
    {
        DOS_LOG_WARN("the parameter is incorrect and cannot continue to create a memory pool");
        return DOS_NULL;
    }
    
#if (DOS_HEAP_TO_MEMP == 0U)
    if(base == DOS_NULL)
    {
        DOS_LOG_WARN("memory base addr is null, and cannot continue to create a memory pool");
        return DOS_NULL;
    }
    memp = (Dos_Memp_t)Dos_MemAlloc(sizeof(struct Memp));
    memp_cnt = (memp_size / (blk_size + sizeof(struct MempNode)));
#else
    memp_cnt = (memp_size / blk_size);
    memp = (Dos_Memp_t)Dos_MemAlloc((sizeof(struct Memp)) +(sizeof(struct MempNode) * memp_cnt) + memp_size);
#endif 
    if(memp == DOS_NULL)
    {
       return DOS_NULL; 
    }

#if (DOS_HEAP_TO_MEMP == 0U)
    memp->Base = base;
#else
    memp->Base = (dos_void*)((dos_uint32)memp + sizeof(struct Memp));
#endif 

    memp->Size = memp_size;
    memp->TotalCnt = memp_cnt;
    memp->FreeCnt = memp_cnt;
    
    node = (Dos_MempNode_t)memp->Base;
    node->Memp = (Dos_Memp_t)memp;
    memp->Free.Next = node;
    for(i = 1; i < memp->TotalCnt; i++)
    {
        next_node = (Dos_MempNode_t)((dos_uint32)node + sizeof(struct MempNode) + blk_size);
        node->Next = next_node;
        node->Memp = (Dos_Memp_t)memp;
        node = next_node;
    }
    node->Next = DOS_NULL;
    node->Memp = (Dos_Memp_t)memp;
    return memp;
}


dos_void *Dos_MempAlloc(Dos_Memp_t memp)
{
    Dos_MempNode_t node;
    
    if((memp->FreeCnt <= 0) || (memp->Free.Next == DOS_NULL))
    {
        DOS_LOG_WARN("the number of free memory blocks in the memory pool is 0\n");
        return DOS_NULL;
    }
    
    memp->FreeCnt--;
    
    node = memp->Free.Next;
    node->Status = DOS_MEMP_STATUS_USED;
    
    memp->Free.Next = node->Next;
    
    return (dos_void*)((dos_uint32)node + sizeof(struct MempNode));
}

dos_void Dos_MempFree(dos_void * ptr)
{
    Dos_Memp_t memp;
    Dos_MempNode_t node;
    Dos_MempNode_t next_node;
    
    node = (Dos_MempNode_t)((dos_uint32)ptr - sizeof(struct MempNode));
    memp = (Dos_Memp_t)(node->Memp);
    
    if((node->Status == DOS_MEMP_STATUS_UNUSED) || (memp == DOS_NULL) || (memp->FreeCnt >= memp->TotalCnt))
    {
        DOS_LOG_WARN("the memory block to be free is not in the correct format\n");
        return;
    }
    
    next_node = memp->Free.Next;
    memp->Free.Next = node;
    node->Next = next_node;
    
    memp->FreeCnt++;
    node->Status = DOS_MEMP_STATUS_UNUSED;
}






