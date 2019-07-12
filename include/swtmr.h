#ifndef _SWTMR_H_
#define _SWTMR_H_

#include <dos_def.h>
#include <mem.h>
#include <list.h>
#include <task.h>


enum Dos_Swtmr_Mode
{
    Dos_Swtmr_OnceMode = 0x01,
    Dos_Swtmr_PeriodMode = 0x02,
    Dos_Swtmr_ModeMask = 0x03,
};

enum Dos_Swtmr_Option
{
    Dos_Swtmr_OpStart = 0x01,
    Dos_Swtmr_OpStop = 0x02,
    Dos_Swtmr_OpDelete = 0x04,
    Dos_Swtmr_OpReset = 0x08,     /** This operation option is reserved */
    Dos_Swtmr_OpMask = 0x0f,
};

#define     DOS_SWTMR_STATUS_UNUSED     0x00
#define     DOS_SWTMR_STATUS_CREATE     0x01
#define     DOS_SWTMR_STATUS_RUNNING    0x02
#define     DOS_SWTMR_STATUS_STOP       0x04


typedef dos_void (*Swtmr_CallBacke)(dos_void *Parameter);

struct Dos_Swtmr
{
    struct Dos_Swtmr    *Prev;
    struct Dos_Swtmr    *Next;
    dos_void            *Parameter;      /** Parameter */
    Swtmr_CallBacke     CallBacke;
    dos_uint16          Status;
    dos_uint16          Mode;
    dos_uint32          Timeout;
    dos_uint32          WakeTime;
};
typedef struct Dos_Swtmr * Dos_Swtmr_t;

struct Dos_SwtmrMsg
{
    dos_uint32          Option;
    Dos_Swtmr_t         Swtmr;
};
typedef struct Dos_SwtmrMsg * Dos_SwtmrMsg_t;

dos_err Dos_SwtmrInit(void);
Dos_Swtmr_t Dos_SwtmrCreate(dos_uint32 timeout, dos_uint16 mode, Swtmr_CallBacke cb, dos_void *param);

#endif // !_SWTMR_H_
