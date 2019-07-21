
#ifndef _MEMP_H_
#define _MEMP_H_

#include <dos_def.h>
#include <dos_config.h>

#define     DOS_MEMP_STATUS_UNUSED      0x00
#define     DOS_MEMP_STATUS_USED        0x01

struct MempNode {
    dos_uint32 Status;
    struct MempNode *Next;
};
typedef struct MempNode * Dos_MempNode_t;

struct Memp {
    /** Element size */
    dos_uint32 Size;

    /** count of total memory block */
    dos_uint32 TotalCnt;

    /** count of free memory block */
    dos_uint32 FreeCnt;

    /** Base address */
    dos_void *Base;

    /** First free memory block of memory pool. memory block form a linked list. */
    struct MempNode Free;
};
typedef struct Memp * Dos_Memp_t;

Dos_Memp_t Dos_MempCreate(
#if (DOS_HEAP_TO_MEMP == 0U)
    dos_void *base,
#endif 
    dos_uint32 memp_size,
    dos_uint32 blk_size);
dos_void *Dos_MempAlloc(Dos_Memp_t memp);

#endif // !_MEMP_H


