#ifndef __LIST_H_
#define __LIST_H_

#include <dos_def.h>


typedef struct DOS_DList
{
    struct DOS_DList *Prev;      /**< Current node's pointer to the previous node*/
    struct DOS_DList *Next;      /**< Current node's pointer to the next node*/
} DOS_DList_t;


void Dos_DListInit(DOS_DList_t *dos_dlist);
void Dos_DListInser(DOS_DList_t *dlist , DOS_DList_t *new_dlist);
void Dos_DListDel(DOS_DList_t *dlist);


#endif /* __REDEF_H */

/********************************END OF FILE***************************************/

