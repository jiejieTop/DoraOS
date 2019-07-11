#ifndef _EVENT_H_
#define _EVENT_H_
#include <dos_def.h>
#include <mem.h>
#include <list.h>
#include <task.h>


#define     WAIT_EVENT_OP       0x03        /** waiting event options mask */
#define     WAIT_ANY_EVENT      0x01        /** waiting any event */
#define     WAIT_ALL_EVENT      0x02        /** waiting all event */
#define     NO_CLR_EVENT        0x04        /** do not clear events */

struct Dos_Event
{
    dos_uint32      EventSet;       /** Event Set */
    Dos_TaskList_t  EventPend;      /** task pend list, 28 byte */
};
typedef struct Dos_Event * Dos_Event_t;


Dos_Event_t Dos_EventCreate(void);
dos_err Dos_EventDelete(Dos_Event_t event);
dos_uint32 Dos_EventWait(Dos_Event_t event, dos_uint32 wait_event, dos_uint32 op, dos_uint32 timeout);
dos_uint32 Dos_EventSet(Dos_Event_t event, dos_uint32 set_event);

#endif // !_EVENT_H_

