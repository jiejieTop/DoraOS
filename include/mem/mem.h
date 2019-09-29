#ifndef __MEM_H_
#define __MEM_H_
#include <dos_def.h>
#include <list.h>

extern void dos_scheduler_lock(void);
extern void dos_scheduler_unlock(void);

/* Memory node information control block */
typedef struct dos_mem_heap_node
{
  struct dos_mem_heap_node* prev;   
  dos_uint32 mem_node_size : 30 ;  /* mem node size */
  dos_uint32 mem_used : 1;         /* used or no */
  dos_uint32 mem_align : 1;        /* align or no, reserved */
  dos_uint8  user_mem[];
}dos_mem_heap_node_t;

/* Memory heap management information control block */
typedef struct dos_mem_heap_info
{
  dos_void *mem_heap_addr;         /* Starting address of memory heap */
  dos_uint32 mem_heap_size;        /* Memory Heap size */
  struct dos_mem_heap_node *MemHead;   
  struct dos_mem_heap_node *mem_tail;   
}dos_mem_heap_info_t;


#define   MEM_INFO_SIZE   sizeof(struct dos_mem_heap_info)
#define   MEM_NODE_SIZE   sizeof(struct dos_mem_heap_node)
  
/* Memory alignment */
#define DOS_ALIGN(addr, size)           (((addr) + (size - 1)) & ~((size) - 1))

#define DOS_ALIGN_DOWN(addr, size)      ((addr) & ~((size) - 1))

dos_bool dos_mem_heap_init(void);
dos_void* dos_mem_alloc(dos_uint32 size);
dos_err dos_mem_free(void *mem);
dos_uint32 dos_get_mem_heap_info(void);

#endif /* __REDEF_H */


/********************************END OF FILE***************************************/

