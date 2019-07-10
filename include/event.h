#ifndef _EVENT_H_
#define _EVENT_H_



struct Dos_Event
{
    dos_uint32      EventSet;       /** Event Set */
    Dos_TaskList_t  EventPend;      /** task pend list, 28 byte */
};
typedef struct Dos_Event * Dos_Event_t;




#endif // !_EVENT_H_