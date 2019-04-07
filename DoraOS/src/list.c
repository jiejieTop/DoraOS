#include <list.h>

/* double list init */
void Dos_DListInit(DOS_DList_t *dos_dlist)
{
  dos_dlist->Next = dos_dlist;
  dos_dlist->Prev = dos_dlist;
}



/* inser a new node in double list */
void Dos_DListInser(DOS_DList_t *dos_dlist , DOS_DList_t *new_dlist)
{
  new_dlist->Next = dos_dlist->Next;
  new_dlist->Prev = dos_dlist;
  dos_dlist->Next->Prev = new_dlist;
  dos_dlist->Next = new_dlist;
}

/* delete a node in double list */
void Dos_DListDel(DOS_DList_t *dos_dlist)
{
    dos_dlist->Next->Prev = dos_dlist->Prev;
    dos_dlist->Prev->Next = dos_dlist->Next;
    dos_dlist->Next = (DOS_DList_t *)dos_dlist;
    dos_dlist->Prev = (DOS_DList_t *)dos_dlist;
}



/* double list init */
void Dos_TaskList_Init(Dos_TaskList_t *dos_tasklist)
{
  Dos_DListInit(&(dos_tasklist->TaskDList));
  
  dos_tasklist->Item = 0;
  dos_tasklist->TCB_Addr = DOS_NULL;
  
}



