/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-07-15 23:56:23
 * @LastEditTime: 2019-12-05 23:03:24
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
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
dos_memp_t dos_memp_create(
#if (DOS_HEAP_TO_MEMP == 0U)
    dos_void *base,
#endif
    dos_uint32 memp_size,
    dos_uint32 blk_size)
{
    dos_uint32 i,memp_cnt;
    dos_memp_t memp;
    dos_memp_node_t node;
    dos_memp_node_t next_node;

    if ((blk_size == 0) || (memp_size == 0) || (memp_size < blk_size)) {
        DOS_LOG_WARN("the parameter is incorrect and cannot continue to create a memory pool");
        return DOS_NULL;
    }

#if (DOS_HEAP_TO_MEMP == 0U)
    if (base == DOS_NULL) {
        DOS_LOG_WARN("memory base addr is null, and cannot continue to create a memory pool");
        return DOS_NULL;
    }
    memp = (dos_memp_t)dos_mem_alloc(sizeof(struct dos_memp));
    memp_cnt = (memp_size / (blk_size + sizeof(struct dos_memp_node)));
#else
    memp_cnt = (memp_size / blk_size);
    memp = (dos_memp_t)dos_mem_alloc((sizeof(struct dos_memp)) +(sizeof(struct dos_memp_node) * memp_cnt) + memp_size);
#endif
    if (memp == DOS_NULL) {
        return DOS_NULL;
    }

#if (DOS_HEAP_TO_MEMP == 0U)
    memp->memp_base = base;
#else
    memp->memp_base = (dos_void*)((dos_uint32)memp + sizeof(struct dos_memp));
#endif

    memp->memp_size = memp_size;
    memp->memp_total_count = memp_cnt;
    memp->memp_free_count = memp_cnt;

    node = (dos_memp_node_t)memp->memp_base;
    node->dos_memp = (dos_memp_t)memp;
    memp->memp_free.next = node;
    for (i = 1; i < memp->memp_total_count; i++) {
        next_node = (dos_memp_node_t)((dos_uint32)node + sizeof(struct dos_memp_node) + blk_size);
        node->next = next_node;
        node->dos_memp = (dos_memp_t)memp;
        node = next_node;
    }
    node->next = DOS_NULL;
    node->dos_memp = (dos_memp_t)memp;
    return memp;
}


dos_void *dos_memp_alloc(dos_memp_t memp)
{
    dos_memp_node_t node;

    if ((memp->memp_free_count <= 0) || (memp->memp_free.next == DOS_NULL)) {
        DOS_LOG_WARN("the number of free memory blocks in the memory pool is 0\n");
        return DOS_NULL;
    }

    memp->memp_free_count--;

    node = memp->memp_free.next;
    node->status = DOS_MEMP_STATUS_USED;

    memp->memp_free.next = node->next;

    return (dos_void*)((dos_uint32)node + sizeof(struct dos_memp_node));
}

dos_void dos_memp_free(dos_void * ptr)
{
    dos_memp_t memp;
    dos_memp_node_t node;
    dos_memp_node_t next_node;

    node = (dos_memp_node_t)((dos_uint32)ptr - sizeof(struct dos_memp_node));
    memp = (dos_memp_t)(node->dos_memp);

    if ((node->status == DOS_MEMP_STATUS_UNUSED) || (memp == DOS_NULL) || (memp->memp_free_count >= memp->memp_total_count)) {
        DOS_LOG_WARN("the memory block to be free is not in the correct format\n");
        return;
    }

    next_node = memp->memp_free.next;
    memp->memp_free.next = node;
    node->next = next_node;

    memp->memp_free_count++;
    node->status = DOS_MEMP_STATUS_UNUSED;
}






