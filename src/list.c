#include <list.h>

/* double list init */
void Dos_TaskItem_Init(Dos_TaskItem_t *dos_item)
{
  dos_item->Next = DOS_NULL;
  dos_item->Prev = DOS_NULL;
  dos_item->Dos_TaskValue = 0;
  dos_item->Dos_TCB = DOS_NULL;
  dos_item->Dos_TaskList = DOS_NULL;
}



/* insert a new node in double list */
void Dos_TaskItem_insert(Dos_TaskList_t *dos_list , Dos_TaskItem_t *new_item)
{
  Dos_TaskItem_t *item;
  dos_uint32 value = new_item->Dos_TaskValue;
  
  for(item = (Dos_TaskItem_t*)&(dos_list->Task_EndItem);
      item->Next->Dos_TaskValue <= value;
      item = item->Next);
  
  new_item->Next = item->Next;
  new_item->Next->Prev = new_item;
  new_item->Prev = item;
  item->Next = new_item;
  
  new_item->Dos_TaskList = (dos_void*)dos_list;
  
  dos_list->Task_ItemValue++;
}

/* delete a node in double list */
dos_uint32 Dos_TaskItem_Del(Dos_TaskItem_t *dos_item)
{
  Dos_TaskList_t *dos_list = dos_item->Dos_TaskList;
  
  dos_item->Prev->Next = dos_item->Next;
  dos_item->Next->Prev = dos_item->Prev;
  
  dos_item->Dos_TaskList = DOS_NULL;
  
  if(dos_list->Dos_TaskItem == dos_item)
  {
    dos_list->Dos_TaskItem = dos_item->Prev;
  }
  
  dos_list->Task_ItemValue--;
  
  return dos_list->Task_ItemValue;
}

dos_bool Dos_TaskList_IsEmpty(Dos_TaskList_t *dos_tasklist)
{
  return (dos_tasklist->Task_ItemValue == 0) ? DOS_TRUE : DOS_FALSE; 
}


dos_uint32 Dos_Get_TaskListValue(Dos_TaskList_t *dos_tasklist)
{
  return dos_tasklist->Task_ItemValue;
}


/* double list init */
void Dos_TaskList_Init(Dos_TaskList_t *dos_tasklist)
{
  dos_tasklist->Dos_TaskItem = (Dos_TaskItem_t *)&(dos_tasklist->Task_EndItem);
    
  dos_tasklist->Task_ItemValue = 0;
  
  dos_tasklist->Task_EndItem.Dos_TaskValue = 0xFFFFFFFF;
  dos_tasklist->Task_EndItem.Next = (Dos_TaskItem_t *)&(dos_tasklist->Task_EndItem);
  dos_tasklist->Task_EndItem.Prev = (Dos_TaskItem_t *)&(dos_tasklist->Task_EndItem);
  dos_tasklist->Task_EndItem.Dos_TaskList = dos_tasklist;
}



