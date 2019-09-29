#include <mem.h>
#include <log.h>
#include <task.h>
#include <string.h>
#include <dos_config.h>


#ifndef DOS_ALIGN_SIZE
#define DOS_ALIGN_SIZE		(4)
#endif 

#ifndef DOS_MEM_HEAP_SIZE
#define DOS_MEM_HEAP_SIZE   1024*10
#endif 

static dos_uint8 *_align_mem_heap_begin = DOS_NULL;
static dos_uint8 *_align_mem_heap_end = DOS_NULL;

static dos_uint8 _mem_heap[DOS_MEM_HEAP_SIZE];


/**
 * get memory list next memory node 
 */
static dos_mem_heap_node_t* _dos_get_next_node(dos_mem_heap_node_t* node)
{
  dos_mem_heap_info_t *memheap_info = (dos_mem_heap_info_t *)_align_mem_heap_begin;

  if(memheap_info->mem_tail == node) 
    return DOS_NULL;
  else
    return (dos_mem_heap_node_t*)(node->user_mem + node->mem_node_size);
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
dos_void* dos_mem_alloc(dos_uint32 size)
{
  dos_mem_heap_info_t *memheap_info = (dos_mem_heap_info_t *)DOS_NULL;
  dos_mem_heap_node_t *mem_node = (dos_mem_heap_node_t *)DOS_NULL;
  dos_mem_heap_node_t *best_node = (dos_mem_heap_node_t *)DOS_NULL;
  dos_mem_heap_node_t *node = (dos_mem_heap_node_t *)DOS_NULL;
  
  dos_void* result;
  
  /* Scheduler lock */
  dos_scheduler_lock();
  
  memheap_info = (dos_mem_heap_info_t *)_align_mem_heap_begin;
  if(DOS_NULL == memheap_info)
  {
    return DOS_NULL;
  }  
  
  size = DOS_ALIGN(size,DOS_ALIGN_SIZE);  /* align */
  
  mem_node = memheap_info->mem_tail;   /** get mem node */
  
  while(mem_node)   /** When a memory node exists */
  {
    /**If current memory is not in use and the memory node mem_node_size > size, and best memory node  mem_node_size > current memory node mem_node_size */
    if((!mem_node->mem_used) && (mem_node->mem_node_size > size) && (!best_node || best_node->mem_node_size > mem_node->mem_node_size))
    {
      best_node = mem_node;       /** best memory node is current memory node */
      if(best_node->mem_node_size == size)
      {
        goto FIND_BEST_MEM;     /** The best memory node is exactly equal to the memory block size the user needs */
      }
    }
    mem_node = mem_node->prev;  /** traversing the memory list */
  }

  if (!best_node) /*alloc failed*/
  {
    DOS_LOG_ERR("there's not enough whole to alloc %d Bytes!\n", size);
    
    /* Scheduler unlock */
    dos_scheduler_unlock();
    
    return DOS_NULL;
  }
  
  /* Can cut the memory node */
  if(best_node->mem_node_size - size > MEM_NODE_SIZE)  
  {
    mem_node = (dos_mem_heap_node_t *)(best_node->user_mem + size);   /** get a new free memory node address */
    
    mem_node->mem_used = 0;    /** free memory node mem_used flag is set to 0 */
    mem_node->mem_node_size = best_node->mem_node_size - size - MEM_NODE_SIZE;  /** calculate the free memory node size mem_node_size  */
    mem_node->prev = best_node;   /** insert free memory block list */
    
    if(best_node != memheap_info->mem_tail) 
    {
      if(DOS_NULL != (node = _dos_get_next_node(mem_node)))  /** get memory list next node */
        node->prev = mem_node;    /** insert free memory block list */
    }
    else 
      memheap_info->mem_tail = mem_node;   /** set memory heap struct info mem_tail pointer pointing new free memory node*/
  }
  
  
FIND_BEST_MEM:              /** find best memory node */
  best_node->mem_align = 0;  /** reserved */
  best_node->mem_used = 1;   /** memory node is already in use */
  best_node->mem_node_size = size;   /** set memory node info */
  result = best_node->user_mem;
  
  memheap_info->mem_heap_size -= size;

  /* Scheduler unlock */
  dos_scheduler_unlock();

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

dos_err dos_mem_free(void *mem)
{  
  dos_mem_heap_info_t *memheap_info = (dos_mem_heap_info_t *)DOS_NULL;
  dos_mem_heap_node_t *free_node = (dos_mem_heap_node_t *)DOS_NULL;
  dos_mem_heap_node_t *node = (dos_mem_heap_node_t *)DOS_NULL;

  if(mem == DOS_NULL)
  {
    DOS_LOG_ERR("mem is null!\n");
    return DOS_NOK;
  }
    
  memheap_info = (dos_mem_heap_info_t *)_align_mem_heap_begin;    /** get the memory management structure from the memory heap start address */
  if(DOS_NULL == memheap_info)
  {
    DOS_LOG_ERR("mem info struct is null!\n");
    return DOS_NOK;
  }  
  
  /** Find the real address of the memory node by offset */
  free_node = (dos_mem_heap_node_t *)((dos_uint32)mem - MEM_NODE_SIZE);

  if(0 == free_node->mem_used)   /** determine if the memory node is already in use */
  {
    DOS_LOG_ERR("mem is unused\n");
    return DOS_NOK;
  }

  free_node->mem_used = 0;   /** can free memory node , mem_used flag is set to 0 */

  memheap_info->mem_heap_size += free_node->mem_node_size;

  while((DOS_NULL != free_node->prev) && (0 == free_node->prev->mem_used))
  {
    free_node = free_node->prev;    /** if current memory node prev is not null, and the node is free, can merging */
  }

  while(((node = (dos_mem_heap_node_t *)_dos_get_next_node(free_node)) != DOS_NULL) && (0 == node->mem_used))
  {
    free_node->mem_node_size += MEM_NODE_SIZE + node->mem_node_size;  /** reset the mem_node_size of the new memory node */

    if(node == memheap_info->mem_tail)   
      memheap_info->mem_tail = free_node;    /** set mem_tail pointer to the memory management information structure */
  }

  if((node = (dos_mem_heap_node_t *)_dos_get_next_node(free_node)) != DOS_NULL)
    node->prev = free_node;   /** insert free memory block list */
  
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
dos_bool dos_mem_heap_init(void)
{
  dos_mem_heap_info_t *memheap_info = (dos_mem_heap_info_t *)DOS_NULL;
  dos_mem_heap_node_t *memheap_node = (dos_mem_heap_node_t *)DOS_NULL;
  
  dos_uint32 align_memheap_size;
 
  /* Get the begin and end addresses of the memory heap */
  dos_uint32 memheap_addr = (dos_uint32) _mem_heap;
  _align_mem_heap_begin = (dos_uint8 *) DOS_ALIGN(memheap_addr,DOS_ALIGN_SIZE);
  
  _align_mem_heap_end = (dos_uint8 *)memheap_addr + DOS_MEM_HEAP_SIZE;
  _align_mem_heap_end = (dos_uint8 *) DOS_ALIGN_DOWN((dos_uint32)_align_mem_heap_end,DOS_ALIGN_SIZE);
  
  /* Get the actual size of the memory heap */
  align_memheap_size = (dos_uint32)(_align_mem_heap_end - _align_mem_heap_begin);
  
  /* Memory heap management information control block */
  memheap_info = (dos_mem_heap_info_t *)_align_mem_heap_begin;
  
  if((!memheap_info)||(align_memheap_size <= (MEM_INFO_SIZE + MEM_NODE_SIZE)))
    return DOS_FALSE;

  memset(_align_mem_heap_begin, 0, align_memheap_size);
  
  /* Init  */
  memheap_info->mem_heap_addr = _align_mem_heap_begin;
  memheap_info->mem_heap_size = align_memheap_size - MEM_INFO_SIZE - MEM_NODE_SIZE;
  memheap_info->MemHead = (dos_mem_heap_node_t *)(_align_mem_heap_begin + MEM_INFO_SIZE);
  memheap_node = memheap_info->MemHead;
  memheap_info->mem_tail = memheap_node;
  
  /* Init memory node information control block */
  memheap_node->mem_used = 0;
  memheap_node->mem_node_size = memheap_info->mem_heap_size;
  memheap_node->prev = DOS_NULL;

  return DOS_TRUE;
}


/**
 * get the remaining memory information of the current system
 */
dos_uint32 dos_get_mem_heap_info(void)
{
  dos_mem_heap_info_t *memheap_info = (dos_mem_heap_info_t *)DOS_NULL;
  
  /* Memory heap management information control block */
  memheap_info = (dos_mem_heap_info_t *)_align_mem_heap_begin;

  return memheap_info->mem_heap_size;
}


