#include <list.h>

/* double list init */
void Dos_DListInit(DOS_DList_t *dos_dlist)
{
  dos_dlist->Next = dos_dlist;
  dos_dlist->Prev = dos_dlist;
}

/* inser a new node in double list */
void Dos_DListInser(DOS_DList_t *dlist , DOS_DList_t *new_dlist)
{
  new_dlist->Next = dlist->Next;
  new_dlist->Prev = dlist;
  dlist->Next->Prev = new_dlist;
  dlist->Next = new_dlist;
}

/* delete a node in double list */
void Dos_DListDel(DOS_DList_t *dlist)
{
    dlist->Next->Prev = dlist->Prev;
    dlist->Prev->Next = dlist->Next;
    dlist->Next = (DOS_DList_t *)DOS_NULL;
    dlist->Prev = (DOS_DList_t *)DOS_NULL;
}

