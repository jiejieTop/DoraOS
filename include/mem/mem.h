#ifndef __MEM_H_
#define __MEM_H_
#include <dos_def.h>
#include <list.h>

extern void Dos_Scheduler_Lock(void);
extern void Dos_Scheduler_Unlock(void);

/* Memory node information control block */
typedef struct MemHeapNode
{
  struct MemHeapNode* Prev;   
  dos_uint32 MemNode_Size : 30 ;  /* mem node size */
  dos_uint32 MemUsed : 1;         /* used or no */
  dos_uint32 MemAlign : 1;        /* align or no, reserved */
  dos_uint8  UserMem[];
}DOS_MemHeap_Node_t;

/* Memory heap management information control block */
typedef struct MemHeapInfo
{
  dos_void *MemHeap_Addr;         /* Starting address of memory heap */
  dos_uint32 MemHeap_Size;        /* Memory Heap size */
  struct MemHeapNode *MemHead;   
  struct MemHeapNode *MemTail;   
}DOS_MemHeap_Info_t;


#define   MEM_INFO_SIZE   sizeof(struct MemHeapInfo)
#define   MEM_NODE_SIZE   sizeof(struct MemHeapNode)
  
/* Memory alignment */
#define DOS_ALIGN(addr, size)           (((addr) + (size - 1)) & ~((size) - 1))

#define DOS_ALIGN_DOWN(addr, size)      ((addr) & ~((size) - 1))

dos_bool Dos_MemHeap_Init(void);
void* Dos_MemAlloc(dos_uint32 size);
dos_err Dos_MemFree(void *dos_mem);


#endif /* __REDEF_H */


/********************************END OF FILE***************************************/

