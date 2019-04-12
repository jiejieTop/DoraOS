#ifndef __LIST_H_
#define __LIST_H_

#include <dos_def.h>


typedef struct DOS_DList
{
  struct DOS_DList *Prev;      /**< Current node's pointer to the previous node*/
  struct DOS_DList *Next;      /**< Current node's pointer to the next node*/
} DOS_DList_t;


typedef struct Dos_TaskList
{
  DOS_DList_t   TaskDList;
  dos_void   *TCB_Addr;
  dos_uint32 Task_Item_Value;
}Dos_TaskList_t;


void Dos_DListInit(DOS_DList_t *dos_dlist);
void Dos_DListInser(DOS_DList_t *dos_dlist , DOS_DList_t *new_dlist);
void Dos_DListDel(DOS_DList_t *dos_dlist);
void Dos_TaskList_Init(Dos_TaskList_t *dos_tasklist);

#endif /* __REDEF_H */

/********************************END OF FILE***************************************/

