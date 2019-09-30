#include <sys.h>
#include <task.h>
#include <swtmr.h>
#include <salof.h>
#include <dos_config.h>

#if DOS_LOGO_PRINT
#define DOS_LOGO_BIG 0
#define DOS_LOGO_SMALL 1

#ifndef DOS_LOGO
#define DOS_LOGO DOS_LOGO_BIG
#endif // !DOS_LOGO

#if (DOS_LOGO == DOS_LOGO_BIG)
/* This logo contains 2097 characters */
static dos_char _DoraOS_Logo[] = "\r\n \
  DDDDDDDDDDDDD                                                                     OOOOOOOOO         SSSSSSSSSSSSSSS   \r\n \
  D::::::::::::DDD                                                                OO:::::::::OO     SS:::::::::::::::S  \r\n \
  D:::::::::::::::DD                                                            OO:::::::::::::OO  S:::::SSSSSS::::::S  \r\n \
  DDD:::::DDDDD:::::D                                                          O:::::::OOO:::::::O S:::::S     SSSSSSS  \r\n \
    D:::::D    D:::::D     ooooooooooo    rrrrr   rrrrrrrrr    aaaaaaaaaaaaa   O::::::O   O::::::O S:::::S              \r\n \
    D:::::D     D:::::D  oo:::::::::::oo  r::::rrr:::::::::r   a::::::::::::a  O:::::O     O:::::O S:::::S              \r\n \
    D:::::D     D:::::D o:::::::::::::::o r:::::::::::::::::r  aaaaaaaaa:::::a O:::::O     O:::::O  S::::SSSS           \r\n \
    D:::::D     D:::::D o:::::ooooo:::::o rr::::::rrrrr::::::r          a::::a O:::::O     O:::::O   SS::::::SSSSS      \r\n \
    D:::::D     D:::::D o::::o     o::::o  r:::::r     r:::::r   aaaaaaa:::::a O:::::O     O:::::O     SSS::::::::SS    \r\n \
    D:::::D     D:::::D o::::o     o::::o  r:::::r     rrrrrrr aa::::::::::::a O:::::O     O:::::O        SSSSSS::::S   \r\n \
    D:::::D     D:::::D o::::o     o::::o  r:::::r            a::::aaaa::::::a O:::::O     O:::::O             S:::::S  \r\n \
    D:::::D    D:::::D  o::::o     o::::o  r:::::r           a::::a    a:::::a O::::::O   O::::::O             S:::::S  \r\n \
  DDD:::::DDDDD:::::D   o:::::ooooo:::::o  r:::::r           a::::a    a:::::a O:::::::OOO:::::::O SSSSSSS     S:::::S  \r\n \
  D:::::::::::::::DD    o:::::::::::::::o  r:::::r           a:::::aaaa::::::a  OO:::::::::::::OO  S::::::SSSSSS:::::S  \r\n \
  D::::::::::::DDD       oo:::::::::::oo   r:::::r            a::::::::::aa:::a   OO:::::::::OO    S:::::::::::::::SS   \r\n \
  DDDDDDDDDDDDD            ooooooooooo     rrrrrrr             aaaaaaaaaa  aaaa     OOOOOOOOO       SSSSSSSSSSSSSSS     \r\n ";
#else
/* This logo contains 573 characters */
static dos_char _DoraOS_Logo[] = "\r\n \
 Welcome to DoraOS real-time operating system -- jiejie \r\n\n \
 oooooooooo.                                      .oooooo.     .ooooooo.   \r\n \
 `888'   `Y8b                                    d8P'  `Y8b   d8P'    `Y8  \r\n \
  888      888   .ooooo.   oooo d8b  .coooo.    888      888  Y88bo.       \r\n \
  888      888  d88' '88b  `888''8P  `P  )88b   888      888   'Yl8888o.   \r\n \
  888      888  888   888   888       .oP''88   888      888       'Y88lb  \r\n \
  888     d88'  888   888   888      d8(   88   `88b    d88'  oo     .d8P  \r\n \
 o888bood8P'    `Y8bod8P'  d888b     `Y888''8o.  'Y8bood8P'   '8888888P'  \r\n ";
#endif

#ifdef DOS_LOGO
static void _Dos_PrintfLogo(void)
{
	Dos_SalofOut(_DoraOS_Logo, sizeof(_DoraOS_Logo));
}
#endif

#endif

/**
 * system variable
 */
static dos_uint8 _dos_system_is_run = DOS_NO; /** system run flag */
static dos_uint8 _dos_scheduler_lock = 0;	/** scheduler lock number of times*/
static dos_uint8 _dos_interrupt_count = 0;	/** system entry interrupt number of times*/
/**
 * task scheduler(Internally used function)
 */
static dos_bool _dos_scheduler(dos_void)
{
	if ((dos_system_is_running()) && (dos_check_task_priority() != DOS_FALSE))
	{
		return DOS_TRUE;
	}
	return DOS_FALSE;
}



void dos_system_init(void)
{
#if DOS_LOGO_PRINT
	/** printf DoraOS logo */
	_Dos_PrintfLogo();
#endif

	/* system memheap init */
	dos_mem_heap_init();

	Dos_SalofInit();

	dos_task_init();

#if DOS_SWTMR
	dos_swtmr_init();
#endif
}

/**
 * start run system
 */
dos_void dos_system_start_run(dos_void)
{
	dos_check_task_priority();

	dos_choose_highest_priority_task();

	/** Update dos_tick_count and dos_is_run to indicate that the system starts to boot */
	dos_reset_tick();

	dos_set_system_status(DOS_YES);

	if (dos_port_start() != 0)
	{
		/* Will not be executed here after the task is started */
	}
}

void dos_set_system_status(dos_uint8 status)
{
	_dos_system_is_run = status;
}

dos_bool dos_system_is_running(void)
{
	return (_dos_system_is_run == DOS_YES);
}

/**
 * system scheduler
 */
dos_void dos_scheduler(dos_void)
{
	if(dos_is_in_interrupt())
		return;
	
	if ((!dos_scheduler_is_lock()) && (_dos_scheduler() == DOS_TRUE))
	{
		DOS_TASK_YIELD();
	}
}

/**
 * Scheduler lock
 */
dos_void dos_scheduler_lock(dos_void)
{
	dos_uint32 pri;

	pri = dos_interrupt_disable();

	_dos_scheduler_lock++;

	dos_interrupt_enable(pri);
}

/**
 * Scheduler unlock
 */
dos_void dos_scheduler_unlock(dos_void)
{
	dos_uint32 pri;

	pri = dos_interrupt_disable();

	if (_dos_scheduler_lock > 0)
	{
		_dos_scheduler_lock--;

		/** When the scheduler is completely unlocked */
		if (0 == _dos_scheduler_lock)
		{
			dos_interrupt_enable(pri);
			/** Perform a task scheduling */
			dos_scheduler();
			return;
		}
	}

	dos_interrupt_enable(pri);
}


/**
 * Determine if the system scheduler is locked
 */
dos_bool dos_scheduler_is_lock(dos_void)
{
	return (0 != _dos_scheduler_lock);
}


dos_bool dos_is_in_interrupt(dos_void)
{
	return (0 != _dos_interrupt_count);
}


void dos_interrupt_enter(void)
{
	dos_uint32 pri;

	pri = dos_interrupt_disable();

    if((!dos_system_is_running()) || (!dos_context_is_interrupt()))
	{
		dos_interrupt_enable(pri);
		return;
	}

    ++_dos_interrupt_count;
	
	dos_interrupt_enable(pri);
}


void dos_interrupt_leave(void)
{
	dos_uint32 pri;

	pri = dos_interrupt_disable();

    if((!dos_system_is_running()) || (!dos_context_is_interrupt()))
	{
		dos_interrupt_enable(pri);
		return;
	}

	--_dos_interrupt_count;

	if(dos_is_in_interrupt())
	{
		dos_interrupt_enable(pri);
		return;
	}

	dos_interrupt_enable(pri);

	dos_scheduler();
}





