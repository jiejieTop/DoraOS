/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2019-07-15 23:56:23
 * @LastEditTime: 2019-12-05 23:03:54
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include <swtmr.h>
#include <log.h>
#include <mem.h>
#include <string.h>
#include <dos_config.h>
#include <task.h>
#include <port.h>
#include <queue.h>

#if DOS_SWTMR

static dos_queue_t _dos_swtmr_queue = DOS_NULL;
static dos_task_t _dos_swtmr_tcb = DOS_NULL;

static struct dos_swtmr _dos_swtmr_list1;
static struct dos_swtmr _dos_swtmr_list2;
static struct dos_swtmr *_dos_swtmr_overflow_ptr;
static struct dos_swtmr *_dos_swtmr_ptr;


static dos_void _dos_swtmr_switch_ptr(void)
{
    static struct dos_swtmr * swtmr_list;
    swtmr_list = _dos_swtmr_overflow_ptr;
    _dos_swtmr_overflow_ptr =_dos_swtmr_ptr;
    _dos_swtmr_ptr = swtmr_list;
}

static void _dos_swtmr_start(dos_swtmr_t swtmr)
{
    dos_swtmr_t swtmr_item;
    dos_uint32 cur_time;

    cur_time = dos_get_tick();

    swtmr->swtmr_wake_time = cur_time + swtmr->swtmr_timeout;

    if (swtmr->swtmr_wake_time < cur_time) { /** overflow */
        for (swtmr_item = _dos_swtmr_overflow_ptr;
             (swtmr_item->swtmr_next->swtmr_wake_time) <= swtmr->swtmr_wake_time;
             swtmr_item = swtmr_item->swtmr_next);
    } else if (swtmr->swtmr_wake_time == 0xFFFFFFFF) { /** Insert the start of the overflow list */
        swtmr_item = _dos_swtmr_ptr->swtmr_prev;
    } else { /** at normal value */
        for (swtmr_item = _dos_swtmr_ptr;
             (swtmr_item->swtmr_next->swtmr_wake_time) <= swtmr->swtmr_wake_time;
             swtmr_item = swtmr_item->swtmr_next);
    }

    swtmr->swtmr_next = swtmr_item->swtmr_next;
    swtmr->swtmr_next->swtmr_prev = swtmr;
    swtmr->swtmr_prev = swtmr_item;
    swtmr_item->swtmr_next = swtmr;

    swtmr->swtmr_status = DOS_SWTMR_STATUS_RUNNING;
}


static void _dos_swtmr_stop(dos_swtmr_t swtmr)
{
    if (_dos_swtmr_ptr->swtmr_next == swtmr) {
        _dos_swtmr_ptr->swtmr_next = swtmr->swtmr_next;
    }

    swtmr->swtmr_prev->swtmr_next = swtmr->swtmr_next;
    swtmr->swtmr_next->swtmr_prev = swtmr->swtmr_prev;

    swtmr->swtmr_status = DOS_SWTMR_STATUS_STOP;
    swtmr->swtmr_prev = swtmr;
    swtmr->swtmr_next = swtmr;
}


static dos_err _dos_swtmr_delete(dos_swtmr_t swtmr)
{
    dos_err err = DOS_NOK;

    if (DOS_NULL == swtmr) {
        DOS_LOG_ERR("unable to delete, swtmr is null\n");
        return DOS_NOK;
    }

    switch (swtmr->swtmr_status) {
    case DOS_SWTMR_STATUS_UNUSED:
        goto out;

    case DOS_SWTMR_STATUS_CREATE:
    case DOS_SWTMR_STATUS_STOP:
        goto delete;

    case DOS_SWTMR_STATUS_RUNNING:
        goto stop;
    }

stop:
    _dos_swtmr_stop(swtmr);
delete:
    dos_mem_free(swtmr);
    memset(swtmr, 0, sizeof(struct dos_swtmr));
    err = DOS_OK;
out:
    return err;
}

static dos_err _dos_swtmr_make_msg(dos_swtmr_t swtmr, dos_uint32 op)
{
    struct dos_swtmr_msg swtmr_msg;
    if ((swtmr == DOS_NULL) && (op != dos_swtmr_opt_overflow) ) {
        DOS_LOG_WARN("the software timer to be started is null\n");
        return DOS_NOK;
    }

    swtmr_msg.swtmr_opt = op;
    swtmr_msg.swtmr = swtmr;

    return dos_queue_write(_dos_swtmr_queue, &swtmr_msg, sizeof(struct dos_swtmr_msg), 0);
}

dos_uint32 dos_get_swtmr_wake_time(void)
{
    return _dos_swtmr_ptr->swtmr_next->swtmr_wake_time;
}

static dos_swtmr_t _dos_get_swtmr(void)
{
    return _dos_swtmr_ptr->swtmr_next;
}

static dos_void _dos_swtmr_timeout_handle(dos_swtmr_t swtmr)
{
    if (swtmr->swtmr_call_backe != DOS_NULL) {
        swtmr->swtmr_call_backe(swtmr->swtmr_parameter);
        _dos_swtmr_stop(swtmr);
        if (swtmr->swtmr_mode == dos_swtmr_mode_period) {
            _dos_swtmr_start(swtmr);
        } else {
            _dos_swtmr_delete(swtmr);
        }
    }
}


static dos_void _dos_swtmr_cmd_handle(dos_swtmr_msg_t msg)
{
    if (msg->swtmr->swtmr_status == DOS_SWTMR_STATUS_UNUSED) {
        DOS_LOG_WARN("the software timer to be operated is unused\n");
        return;
    }
    switch (msg->swtmr_opt) {
    case dos_swtmr_opt_start:
        if (msg->swtmr->swtmr_status == DOS_SWTMR_STATUS_RUNNING) {
            _dos_swtmr_stop(msg->swtmr);
        }
        _dos_swtmr_start(msg->swtmr);
        break;

    case dos_swtmr_opt_stop:
        if (msg->swtmr->swtmr_status == DOS_SWTMR_STATUS_RUNNING) {
            _dos_swtmr_stop(msg->swtmr);
        }
        break;

    case dos_swtmr_opt_overflow:
        _dos_swtmr_switch_ptr();
        break;

    case dos_swtmr_opt_delete:
        _dos_swtmr_delete(msg->swtmr);
        break;

    default:
        break;
    }
}


static void _dos_swtmr_task(void *parameter)
{
    dos_err err;
    dos_swtmr_t wake_swtmr;
    dos_uint32 cur_time, wait_time;
    struct dos_swtmr_msg swtmr_msg;

    for ( ; ; ) {
        wake_swtmr = _dos_get_swtmr();
        cur_time = dos_get_tick();

        if (wake_swtmr == _dos_swtmr_ptr) {
            wait_time = DOS_WAIT_FOREVER;
        } else {
            wait_time = wake_swtmr->swtmr_wake_time-cur_time;
        }

        err = dos_queue_read(_dos_swtmr_queue, &swtmr_msg, sizeof(struct dos_swtmr_msg), wait_time);
        if (err == DOS_NOK) { /** software timer timeout */
            cur_time = dos_get_tick();
            while (cur_time >= wake_swtmr->swtmr_wake_time) {
                _dos_swtmr_timeout_handle(wake_swtmr);
                wake_swtmr = _dos_get_swtmr();
                cur_time = dos_get_tick();
            }
        } else {
            _dos_swtmr_cmd_handle(&swtmr_msg);
        }
    }
}

/********************************************************************************************************/

dos_void _Dos_Swtmr_ItemInit(dos_swtmr_t swtmr)
{
    memset(swtmr, 0, sizeof(struct dos_swtmr));
    swtmr->swtmr_status = DOS_SWTMR_STATUS_UNUSED;
    swtmr->swtmr_prev = (dos_swtmr_t)(swtmr);
    swtmr->swtmr_next = (dos_swtmr_t)(swtmr);
    swtmr->swtmr_wake_time = 0xFFFFFFFF;
}

dos_err dos_swtmr_init(void)
{
#ifndef DOS_IPC_QUEUQ
#error "message queue must be turned on when using software timer"
#endif // !DOS_IPC_QUEUQ

    _dos_swtmr_queue = dos_queue_create(DOS_SWTMR_QUEUE_SIZE, sizeof(struct dos_swtmr_msg));
    if (_dos_swtmr_queue == DOS_NULL) {
        DOS_LOG_ERR("unable to create software timer queue\n");
        return DOS_NOK;
    }

    _dos_swtmr_tcb = dos_task_create("Swtmr_Task",
                                     &_dos_swtmr_task,
                                     DOS_NULL,
                                     DOS_SWTMR_TASK_SIZE,
                                     DOS_SWTMR_TASK_PRIORITY,
                                     DOS_SWTMR_TASK_TICK);
    if (_dos_swtmr_tcb == DOS_NULL) {
        DOS_LOG_ERR("Unable to create software timer task\n");
        return DOS_NOK;
    }

    _Dos_Swtmr_ItemInit(&_dos_swtmr_list1);
    _Dos_Swtmr_ItemInit(&_dos_swtmr_list2);

    _dos_swtmr_ptr = &_dos_swtmr_list1;
    _dos_swtmr_overflow_ptr = &_dos_swtmr_list2;

    return DOS_OK;
}



dos_swtmr_t dos_swtmr_create(dos_uint32 timeout, dos_uint16 mode, dos_swtmr_call_backe cb, dos_void *param)
{
    dos_swtmr_t swtmr;

    swtmr = (dos_swtmr_t)dos_mem_alloc(sizeof(struct dos_swtmr));
    if (swtmr == DOS_NULL) {
        DOS_LOG_ERR("not enough memory to create a software timer\n");
        return DOS_NULL;
    }

    if ((cb == DOS_NULL) || (timeout == 0)) {
        DOS_LOG_WARN("parameter is is invalid\n");
        return DOS_NULL;
    }

    if ((!(mode & dos_swtmr_mode_mask)) || ((mode & dos_swtmr_mode_mask) == dos_swtmr_mode_mask)) {
        DOS_LOG_WARN("parameter is is invalid\n");
        return DOS_NULL;
    }

    swtmr->swtmr_status = DOS_SWTMR_STATUS_CREATE;
    swtmr->swtmr_prev = swtmr;
    swtmr->swtmr_next = swtmr;
    swtmr->swtmr_wake_time = 0;
    swtmr->swtmr_timeout = timeout;
    swtmr->swtmr_mode = mode;
    swtmr->swtmr_call_backe = cb;
    swtmr->swtmr_parameter = param;

    return swtmr;
}


dos_err dos_swtmr_start(dos_swtmr_t swtmr)
{
    return _dos_swtmr_make_msg(swtmr, dos_swtmr_opt_start);
}

dos_err dos_swtmr_stop(dos_swtmr_t swtmr)
{
    return _dos_swtmr_make_msg(swtmr, dos_swtmr_opt_stop);
}

dos_err dos_swtmr_delete(dos_swtmr_t swtmr)
{
    return _dos_swtmr_make_msg(swtmr, dos_swtmr_opt_delete);
}

dos_err dos_swtmr_overflow(void)
{
    return _dos_swtmr_make_msg(DOS_NULL, dos_swtmr_opt_overflow);
}


#endif







