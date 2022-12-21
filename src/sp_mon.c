#include "sp_mon.h"

extern uint32_t _sstack;

static unsigned int _sp_buckets[SP_MON_NUM_BUCKETS];
static bool _enable = false;


void sp_mon_enable(bool enable)
{
	_enable = enable;
	if (_enable)
	{
		int i;
		for (i = 0; i < SP_MON_NUM_BUCKETS; i++)
		{
			_sp_buckets[i] = 0;
		}
	}
}

void sp_mon_check()
{
	if (!_enable)
	{
		return;
	}

	register uint32_t sp asm("r4");
	asm volatile ("mov r4, sp");
	_sp_buckets[(sp - (uint32_t)(&_sstack)) >> SP_MON_BUCKET_SIZE_BITS]++;
}

unsigned int* sp_mon_get_buckets()
{
	return _sp_buckets;
}
