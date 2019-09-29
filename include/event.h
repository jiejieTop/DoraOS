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

struct dos_event
{
    dos_uint32      event_flag;       /** Event Set */
    dos_task_list_t  event_pend_list;      /** task pend list, 28 byte */
};
typedef struct dos_event * dos_event_t;


dos_event_t dos_event_create(void);
dos_err dos_event_delete(dos_event_t event);
dos_uint32 dos_event_wait(dos_event_t event, dos_uint32 wait_event, dos_uint32 op, dos_uint32 timeout);
dos_uint32 dos_event_set(dos_event_t event, dos_uint32 set_event);

#endif // !_EVENT_H_

