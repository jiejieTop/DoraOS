#include <mem.h>
#include "include.h"


#ifndef DOS_ALIGN_SIZE
#define DOS_ALIGN_SIZE		(8)
#endif 

#ifndef MEM_HEAP_SIZE
#define MEM_HEAP_SIZE   1024*10
#endif 

static dos_uint8 *_Align_MemHeap_Begin = DOS_NULL;
static dos_uint8 *_Align_MemHeap_End = DOS_NULL;

static dos_uint8 _MemHeap[MEM_HEAP_SIZE];

static DOS_MemHeap_Node_t* _Dos_Get_NextNode(DOS_MemHeap_Node_t* node);

/**
 * @brief       NULL  
 * @param[in]   NULL
 * @param[out]  NULL
 * @return      NULL
 * @author      jiejie
 * @github      https://github.com/jiejieTop
 * @date        2018-xx-xx
 * @version     v1.0
 * @note        alloc system mem , reference from LiteOS
 */
void* Dos_MemAlloc(dos_uint32 size)
{
  DOS_MemHeap_Info_t *memheap_info = (DOS_MemHeap_Info_t *)DOS_NULL;
  DOS_MemHeap_Node_t *mem_node = (DOS_MemHeap_Node_t *)DOS_NULL;
  DOS_MemHeap_Node_t *best_node = (DOS_MemHeap_Node_t *)DOS_NULL;
  DOS_MemHeap_Node_t *node = (DOS_MemHeap_Node_t *)DOS_NULL;
  
  void* result;
  
  /* 此处需要锁定调度器 */
  memheap_info = (DOS_MemHeap_Info_t *)_Align_MemHeap_Begin;
  if(!memheap_info)
  {
    return DOS_NULL;
  }  
  
  size = DOS_ALIGN(size,DOS_ALIGN_SIZE);  /* align */
  
  mem_node = memheap_info->MemTail;
  
  while(mem_node)
  {
    if((!mem_node->MemUsed) && (mem_node->MemNode_Size > size) && (!best_node || best_node->MemNode_Size > mem_node->MemNode_Size))
    {
      best_node = mem_node;
      if(best_node->MemNode_Size == size)
      {
        goto FIND_BEST_MEM;
      }
    }
    mem_node = mem_node->Prev;
  }

  if (!best_node) /*alloc failed*/
  {
    DOS_PRINT_ERR("there's not enough whole to alloc %x Bytes!\n",size);
    
    /* 此处需要解锁调度器 */
    
    return DOS_NULL;
  }
  
  /* Can cut the memory heap */
  if(best_node->MemNode_Size - size > MEM_NODE_SIZE)  
  {
    mem_node = (DOS_MemHeap_Node_t *)(best_node->UserMem + size);
    
    mem_node->MemUsed = 0;
    mem_node->MemNode_Size = best_node->MemNode_Size - size - MEM_NODE_SIZE;
    mem_node->Prev = best_node;
    
    if(best_node != memheap_info->MemTail)
    {
      if(DOS_NULL != (node = _Dos_Get_NextNode(mem_node)))
        node->Prev = mem_node;
    }
    else 
      memheap_info->MemTail = mem_node;
  }
  
  
FIND_BEST_MEM:
  best_node->MemAlign = 0;
  best_node->MemNode_Size = 1;
  result = best_node->UserMem;
  
    /* 此处需要解锁调度器 */
  
  return result;  
}

/**
 * @brief       NULL  
 * @param[in]   dos_mem
 * @param[out]  NULL
 * @return      NULL
 * @author      jiejie
 * @github      https://github.com/jiejieTop
 * @date        2018-xx-xx
 * @version     v1.0
 * @note        free mem , reference from LiteOS
 */

void Dos_MemFree(void *dos_mem)
{  
  DOS_MemHeap_Info_t *memheap_info = (DOS_MemHeap_Info_t *)DOS_NULL;
  DOS_MemHeap_Node_t *mem_node = (DOS_MemHeap_Node_t *)DOS_NULL;
  DOS_MemHeap_Node_t *best_node = (DOS_MemHeap_Node_t *)DOS_NULL;
  DOS_MemHeap_Node_t *node = (DOS_MemHeap_Node_t *)DOS_NULL;
  
  if(dos_mem == DOS_NULL)
    return;
  
  memheap_info = (DOS_MemHeap_Info_t *)_Align_MemHeap_Begin;
  if(!memheap_info)
  {
    return ;
  }  
  
  
  
}


/**
 * @brief       _Dos_MemHeap_Init  
 * @param[in]   NULL
 * @param[out]  NULL
 * @return      bool
 * @author      jiejie
 * @github      https://github.com/jiejieTop
 * @date        2018-xx-xx
 * @version     v1.0
 * @note        This is a function that internally initializes the memory heap, only for internal calls.
 */
dos_bool Dos_MemHeap_Init(void)
{
  DOS_MemHeap_Info_t *memheap_info = (DOS_MemHeap_Info_t *)DOS_NULL;
  DOS_MemHeap_Node_t *memheap_node = (DOS_MemHeap_Node_t *)DOS_NULL;
  
  dos_uint32 align_memheap_size;
 
  /* Get the begin and end addresses of the memory heap */
  dos_uint32 memheap_addr = (dos_uint32) _MemHeap;
  _Align_MemHeap_Begin = (dos_uint8 *) DOS_ALIGN(memheap_addr,DOS_ALIGN_SIZE);
  
  _Align_MemHeap_End = (dos_uint8 *)memheap_addr + MEM_HEAP_SIZE;
  _Align_MemHeap_End = (dos_uint8 *) DOS_ALIGN_DOWN((dos_uint32)_Align_MemHeap_End,DOS_ALIGN_SIZE);
  
  /* Get the actual size of the memory heap */
  align_memheap_size = (dos_uint32)(_Align_MemHeap_End - _Align_MemHeap_Begin);
  
  /* Memory heap management information control block */
  memheap_info = (DOS_MemHeap_Info_t *)_Align_MemHeap_Begin;
  
  if((!memheap_info)||(align_memheap_size <= (MEM_INFO_SIZE + MEM_NODE_SIZE)))
    return DOS_FALSE;

  memset(_Align_MemHeap_Begin, 0, align_memheap_size);
  
  /* Init  */
  memheap_info->MemHeap_Addr = _Align_MemHeap_Begin;
  memheap_info->MemHeap_Size = align_memheap_size;
  memheap_info->MemHead = (DOS_MemHeap_Node_t *)(_Align_MemHeap_Begin + MEM_INFO_SIZE);
  memheap_node = memheap_info->MemHead;
  memheap_info->MemTail = memheap_node;
  
  /* Init memory node information control block */
  memheap_node->MemUsed = 0;
  memheap_node->MemNode_Size = align_memheap_size - MEM_INFO_SIZE - MEM_NODE_SIZE;
  memheap_node->Prev = DOS_NULL;
  
  return DOS_TRUE;
}


static DOS_MemHeap_Node_t* _Dos_Get_NextNode(DOS_MemHeap_Node_t* node)
{
  DOS_MemHeap_Info_t *memheap_info = (DOS_MemHeap_Info_t *)_Align_MemHeap_Begin;

  if(memheap_info->MemTail == node) 
    return DOS_NULL;
  else
    return (DOS_MemHeap_Node_t*)(node->MemUsed + node->MemNode_Size);
}

