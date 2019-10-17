#ifndef _MEMP_H_
#define _MEMP_H_

#include <dos_def.h>
#include <dos_config.h>

#define     DOS_MEMP_STATUS_UNUSED      0x00
#define     DOS_MEMP_STATUS_USED        0x01

struct dos_memp_node {
    dos_uint32 status;
    dos_void *dos_memp;
    struct dos_memp_node *next;
};
typedef struct dos_memp_node * dos_memp_node_t;

struct dos_memp {
    /** Element size */
    dos_uint32 memp_size;

    /** count of total memory block */
    dos_uint32 memp_total_count;

    /** count of free memory block */
    dos_uint32 memp_free_count;

    /** memp_base address */
    dos_void *memp_base;

    /** First free memory block of memory pool. memory block form a linked list. */
    struct dos_memp_node memp_free;
};
typedef struct dos_memp * dos_memp_t;

dos_memp_t dos_memp_create(
#if (DOS_HEAP_TO_MEMP == 0U)
    dos_void *base,
#endif
    dos_uint32 memp_size,
    dos_uint32 blk_size);
dos_void *dos_memp_alloc(dos_memp_t memp);
dos_void dos_memp_free(dos_void * ptr);

#endif // !_MEMP_H


