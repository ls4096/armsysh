#include <stdbool.h>

#include "timer.h"

#include "armsysh_constants.h"
#include "pm.h"
#include "sp_mon.h"

#include <sam3x8e.h>


static volatile uint32_t _t = 0;

static volatile unsigned int _sleep_counter[2] = { 0, 0 };

#define NOTIFY_COUNT_LIMIT 4
static volatile timer_notify_t* _notify_items[NOTIFY_COUNT_LIMIT];

extern bool _pm_idle;


static void timer_init_hw();


void SysTick_Handler()
{
	_t++;

	_sleep_counter[0]++;
	_sleep_counter[1] += (_pm_idle ? 0 : 1);
	if (_sleep_counter[0] == 0x100)
	{
		pm_update_wake_counter(_sleep_counter[1]);
		_sleep_counter[0] = 0;
		_sleep_counter[1] = 0;
	}

	sp_mon_check();

	int i;
	for (i = 0; i < NOTIFY_COUNT_LIMIT; i++)
	{
		if (_notify_items[i] != 0)
		{
			if (timer_compare(_notify_items[i]->t, _t) < 0)
			{
				_notify_items[i]->notify = true;
				_notify_items[i] = 0;
			}
		}
	}
}


void timer_init()
{
	timer_init_hw();

	int i;
	for (i = 0; i < NOTIFY_COUNT_LIMIT; i++)
	{
		_notify_items[i] = 0;
	}

	pm_reset();
}

uint32_t timer_get_tick_count()
{
	return _t;
}

unsigned char timer_get_tick_count_lsbyte()
{
	return (unsigned char)(_t & 0x000000ff);
}

int timer_compare(uint32_t t0, uint32_t t1)
{
	if (t0 > t1)
	{
		return 1;
	}

	if (t0 < t1)
	{
		return -1;
	}

	return 0;
}

void timer_add_seconds(uint32_t* t, unsigned int seconds)
{
	*t += (seconds * TIMER_TICKS_PER_SECOND);
}

// Assumes t0 > t1.
unsigned int timer_get_diff_seconds(uint32_t t0, uint32_t t1)
{
	return ((t0 - t1) / TIMER_TICKS_PER_SECOND);
}

bool timer_notify_register(timer_notify_t* tn)
{
	int i;
	for (i = 0; i < NOTIFY_COUNT_LIMIT; i++)
	{
		if (_notify_items[i] == 0)
		{
			tn->notify = false;
			_notify_items[i] = tn;

			return true;
		}
	}

	return false;
}

unsigned int timer_get_notify_registered_count()
{
	unsigned int n = 0;

	int i;
	for (i = 0; i < NOTIFY_COUNT_LIMIT; i++)
	{
		if (_notify_items[i] != 0)
		{
			n++;
		}
	}

	return n;
}

static void timer_init_hw()
{
	SysTick->CTRL = 0;
	SysTick->VAL = 0;
	SysTick->LOAD = ((SystemCoreClock / TIMER_TICKS_PER_SECOND) >> 3);
	SysTick->CTRL |= (SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk);

	NVIC_EnableIRQ(SysTick_IRQn);
}
