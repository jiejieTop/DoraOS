#include <list.h>

/* double list init */
void dos_task_item_init(dos_task_item_t *dos_item)
{
  dos_item->next = dos_item;
  dos_item->prev = dos_item;
  dos_item->dos_task_value = 0;
  dos_item->dos_task = DOS_NULL;
  dos_item->dos_task_list = DOS_NULL;
}



/* insert a new node in double list */
void dos_task_item_insert(dos_task_list_t *dos_list , dos_task_item_t *new_item)
{
  dos_task_item_t *item;
  dos_uint32 value = new_item->dos_task_value;
  
  if(value == 0xFFFFFFFF)
  {
    item = dos_list->task_end_item.prev;
  }
  else
  {
    for(item = (dos_task_item_t*)&(dos_list->task_end_item);
        item->next->dos_task_value <= value;
        item = item->next);
  }
  
  new_item->next = item->next;
  new_item->next->prev = new_item;
  new_item->prev = item;
  item->next = new_item;
  
  new_item->dos_task_list = (dos_void*)dos_list;
  
  dos_list->Task_ItemValue++;
}

/* delete a node in double list */
dos_uint32 dos_task_item_del(dos_task_item_t *dos_item)
{
  dos_task_list_t *dos_list = dos_item->dos_task_list;
  
  if(dos_item->dos_task_list != DOS_NULL)
  {
    dos_item->prev->next = dos_item->next;
    dos_item->next->prev = dos_item->prev;
    
    dos_item->dos_task_list = DOS_NULL;
    
    if(dos_list->task_item == dos_item)
    {
      dos_list->task_item = dos_item->prev;
    }
    
    dos_item->next = dos_item;
    dos_item->prev = dos_item;
    
    dos_list->Task_ItemValue--;
  }
  
  return dos_list->Task_ItemValue;
}

dos_bool dos_task_list_is_empty(dos_task_list_t *dos_tasklist)
{
  return (dos_tasklist->Task_ItemValue == 0) ? DOS_TRUE : DOS_FALSE; 
}


dos_uint32 dos_get_task_list_value(dos_task_list_t *dos_tasklist)
{
  return dos_tasklist->Task_ItemValue;
}


/* double list init */
void dos_task_list_init(dos_task_list_t *dos_tasklist)
{
  dos_tasklist->task_item = (dos_task_item_t *)&(dos_tasklist->task_end_item);
    
  dos_tasklist->Task_ItemValue = 0;
  
  dos_tasklist->task_end_item.dos_task_value = 0xFFFFFFFF;
  dos_tasklist->task_end_item.next = (dos_task_item_t *)&(dos_tasklist->task_end_item);
  dos_tasklist->task_end_item.prev = (dos_task_item_t *)&(dos_tasklist->task_end_item);
  dos_tasklist->task_end_item.dos_task_list = dos_tasklist;
}



