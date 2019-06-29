#include <mem.h>
#include <task.h>
#include <string.h>
#include <dos_config.h>


#ifndef DOS_ALIGN_SIZE
#define DOS_ALIGN_SIZE		(8)
#endif 

#ifndef MEM_HEAP_SIZE
#define MEM_HEAP_SIZE   1024*10
#endif 

static dos_uint8 *_Align_MemHeap_Begin = DOS_NULL;
static dos_uint8 *_Align_MemHeap_End = DOS_NULL;

static dos_uint8 _MemHeap[MEM_HEAP_SIZE];


/**
 * get memory list next memory node 
 */
static DOS_MemHeap_Node_t* _Dos_Get_NextNode(DOS_MemHeap_Node_t* node)
{
  DOS_MemHeap_Info_t *memheap_info = (DOS_MemHeap_Info_t *)_Align_MemHeap_Begin;

  if(memheap_info->MemTail == node) 
    return DOS_NULL;
  else
    return (DOS_MemHeap_Node_t*)(node->UserMem + node->MemNode_Size);
}


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
  
  /* Scheduler lock */
  Dos_Scheduler_Lock();
  
  memheap_info = (DOS_MemHeap_Info_t *)_Align_MemHeap_Begin;
  if(DOS_NULL == memheap_info)
  {
    return DOS_NULL;
  }  
  
  size = DOS_ALIGN(size,DOS_ALIGN_SIZE);  /* align */
  
  mem_node = memheap_info->MemTail;   /** get mem node */
  
  while(mem_node)   /** When a memory node exists */
  {
    /**If current memory is not in use and the memory node MemNode_Size > size, and best memory node  MemNode_Size > current memory node MemNode_Size */
    if((!mem_node->MemUsed) && (mem_node->MemNode_Size > size) && (!best_node || best_node->MemNode_Size > mem_node->MemNode_Size))
    {
      best_node = mem_node;       /** best memory node is current memory node */
      if(best_node->MemNode_Size == size)
      {
        goto FIND_BEST_MEM;     /** The best memory node is exactly equal to the memory block size the user needs */
      }
    }
    mem_node = mem_node->Prev;  /** traversing the memory list */
  }

  if (!best_node) /*alloc failed*/
  {
    DOS_PRINT_ERR("there's not enough whole to alloc %x Bytes!\n",size);
    
    /* Scheduler unlock */
    Dos_Scheduler_Unlock();
    
    return DOS_NULL;
  }
  
  /* Can cut the memory node */
  if(best_node->MemNode_Size - size > MEM_NODE_SIZE)  
  {
    mem_node = (DOS_MemHeap_Node_t *)(best_node->UserMem + size);   /** get a new free memory node address */
    
    mem_node->MemUsed = 0;    /** free memory node MemUsed flag is set to 0 */
    mem_node->MemNode_Size = best_node->MemNode_Size - size - MEM_NODE_SIZE;  /** calculate the free memory node size MemNode_Size  */
    mem_node->Prev = best_node;   /** insert free memory block list */
    
    if(best_node != memheap_info->MemTail) 
    {
      if(DOS_NULL != (node = _Dos_Get_NextNode(mem_node)))  /** get memory list next node */
        node->Prev = mem_node;    /** insert free memory block list */
    }
    else 
      memheap_info->MemTail = mem_node;   /** set memory heap struct info MemTail pointer pointing new free memory node*/
  }
  
  
FIND_BEST_MEM:              /** find best memory node */
  best_node->MemAlign = 0;  /** reserved */
  best_node->MemUsed = 1;   /** memory node is already in use */
  best_node->MemNode_Size = size;   /** set memory node info */
  result = best_node->UserMem;
  
  /* Scheduler unlock */
  Dos_Scheduler_Unlock();

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

dos_err Dos_MemFree(void *dos_mem)
{  
  DOS_MemHeap_Info_t *memheap_info = (DOS_MemHeap_Info_t *)DOS_NULL;
  DOS_MemHeap_Node_t *free_node = (DOS_MemHeap_Node_t *)DOS_NULL;
  DOS_MemHeap_Node_t *node = (DOS_MemHeap_Node_t *)DOS_NULL;

  if(dos_mem == DOS_NULL)
  {
    DOS_PRINT_ERR("mem is null!\n");
    return DOS_NOK;
  }
    
  memheap_info = (DOS_MemHeap_Info_t *)_Align_MemHeap_Begin;    /** get the memory management structure from the memory heap start address */
  if(DOS_NULL == memheap_info)
  {
    DOS_PRINT_ERR("mem info struct is null!\n");
    return DOS_NOK;
  }  
  
  /** Find the real address of the memory node by offset */
  free_node = (DOS_MemHeap_Node_t *)((dos_uint32)dos_mem - MEM_NODE_SIZE);

  if(0 == free_node->MemUsed)   /** determine if the memory node is already in use */
  {
    DOS_PRINT_ERR("mem is unused\n");
    return DOS_NOK;
  }

  free_node->MemUsed = 0;   /** can free memory node , MemUsed flag is set to 0 */

  while((DOS_NULL != free_node->Prev) && (0 == free_node->Prev->MemUsed))
  {
    free_node = free_node->Prev;    /** if current memory node Prev is not null, and the node is free, can merging */
  }

  while(((node = (DOS_MemHeap_Node_t *)_Dos_Get_NextNode(free_node)) != DOS_NULL) && (0 == node->MemUsed))
  {
    free_node->MemNode_Size += MEM_NODE_SIZE + node->MemNode_Size;  /** reset the MemNode_Size of the new memory node */

    if(node == memheap_info->MemTail)   
      memheap_info->MemTail = free_node;    /** set MemTail pointer to the memory management information structure */
  }

  if((node = (DOS_MemHeap_Node_t *)_Dos_Get_NextNode(free_node)) != DOS_NULL)
    node->Prev = free_node;   /** insert free memory block list */
  
  return DOS_OK;
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



