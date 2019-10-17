#ifndef _SWTMR_H_
#define _SWTMR_H_

#include <dos_def.h>
#include <mem.h>
#include <list.h>
#include <task.h>


enum dos_swtmr_mode {
    dos_swtmr_mode_one = 0x01,
    dos_swtmr_mode_period = 0x02,
    dos_swtmr_mode_mask = 0x03,
};

enum dos_swtmr_option {
    dos_swtmr_opt_start = 0x01,
    dos_swtmr_opt_stop = 0x02,
    dos_swtmr_opt_delete = 0x04,
    dos_swtmr_opt_overflow = 0x08,
    dos_swtmr_opt_reset = 0x10,     /** This operation option is reserved */
    dos_swtmr_opt_mask = 0xff,
};

#define     DOS_SWTMR_STATUS_UNUSED     0x00
#define     DOS_SWTMR_STATUS_CREATE     0x01
#define     DOS_SWTMR_STATUS_RUNNING    0x02
#define     DOS_SWTMR_STATUS_STOP       0x04


typedef dos_void (*dos_swtmr_call_backe)(dos_void *parameter);

struct dos_swtmr {
    struct dos_swtmr        *swtmr_prev;
    struct dos_swtmr        *swtmr_next;
    dos_void                *swtmr_parameter;      /** parameter */
    dos_swtmr_call_backe    swtmr_call_backe;
    dos_uint16              swtmr_status;
    dos_uint16              swtmr_mode;
    dos_uint32              swtmr_timeout;
    dos_uint32              swtmr_wake_time;
};
typedef struct dos_swtmr * dos_swtmr_t;

struct dos_swtmr_msg {
    dos_uint32          swtmr_opt;
    dos_swtmr_t         swtmr;
};
typedef struct dos_swtmr_msg * dos_swtmr_msg_t;

dos_err dos_swtmr_init(void);
dos_swtmr_t dos_swtmr_create(dos_uint32 timeout, dos_uint16 mode, dos_swtmr_call_backe cb, dos_void *param);
dos_err dos_swtmr_start(dos_swtmr_t swtmr);
dos_err dos_swtmr_stop(dos_swtmr_t swtmr);
dos_err dos_swtmr_delete(dos_swtmr_t swtmr);
dos_err dos_swtmr_overflow(void);
#endif // !_SWTMR_H_

