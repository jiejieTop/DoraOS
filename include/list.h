#ifndef __LIST_H_
#define __LIST_H_

#include <dos_def.h>


struct Dos_TaskItem
{
  struct Dos_TaskItem *Prev;      /**< Current node's pointer to the previous node*/
  struct Dos_TaskItem *Next;      /**< Current node's pointer to the next node*/
  dos_uint32 Dos_TaskValue;
  dos_void *Dos_TCB;
  dos_void *Dos_TaskList;
};
typedef struct Dos_TaskItem Dos_TaskItem_t;

typedef struct Dos_TaskEndItem Dos_TaskEndItem_t;

struct Dos_TaskList
{
  Dos_TaskItem_t *Dos_TaskItem;
  dos_uint32 Task_ItemValue;
  Dos_TaskItem_t Task_EndItem;
};
typedef struct Dos_TaskList Dos_TaskList_t;

void Dos_TaskItem_Init(Dos_TaskItem_t *dos_item);
void Dos_TaskItem_insert(Dos_TaskList_t *dos_list , Dos_TaskItem_t *new_item);
dos_uint32 Dos_TaskItem_Del(Dos_TaskItem_t *dos_item);
dos_bool Dos_TaskList_IsEmpty(Dos_TaskList_t *dos_tasklist);
void Dos_TaskList_Init(Dos_TaskList_t *dos_tasklist);
dos_uint32 Dos_Get_TaskListValue(Dos_TaskList_t *dos_tasklist);

#endif /* __REDEF_H */

/********************************END OF FILE***************************************/

