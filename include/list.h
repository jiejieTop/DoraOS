#ifndef __LIST_H_
#define __LIST_H_

#include <dos_def.h>


struct task_item
{
  struct task_item *prev;      /**< Current node's pointer to the previous node*/
  struct task_item *next;      /**< Current node's pointer to the next node*/
  dos_uint32 dos_task_value;
  dos_void *dos_task;
  dos_void *dos_task_list;
};
typedef struct task_item dos_task_item_t;

typedef struct dos_task_end_item dos_task_end_item_t;

struct dos_task_list
{
  dos_task_item_t *task_item;
  dos_uint32 Task_ItemValue;
  dos_task_item_t task_end_item;
};
typedef struct dos_task_list dos_task_list_t;

void dos_task_item_init(dos_task_item_t *dos_item);
void dos_task_item_insert(dos_task_list_t *dos_list , dos_task_item_t *new_item);
dos_uint32 dos_task_item_del(dos_task_item_t *dos_item);
dos_bool dos_task_list_is_empty(dos_task_list_t *dos_tasklist);
void dos_task_list_init(dos_task_list_t *dos_tasklist);
dos_uint32 dos_get_task_list_value(dos_task_list_t *dos_tasklist);

#endif /* __REDEF_H */

/********************************END OF FILE***************************************/

