#include <stdio.h>

#include "time.h"
#include "timer.h"
#include "util.h"

#define SECONDS_IN_DAY 86400

static bool _set = false;

static uint32_t _timer_nextmid;

bool time_is_set()
{
	return _set;
}

bool time_set_time(const char* str)
{
	if (!util_is_numeric(str[0]) ||
		!util_is_numeric(str[1]) ||
		str[2] != ':' ||
		!util_is_numeric(str[3]) ||
		!util_is_numeric(str[4]) ||
		str[5] != ':' ||
		!util_is_numeric(str[6]) ||
		!util_is_numeric(str[7]))
	{
		return false;
	}

	unsigned char h;
	unsigned char m;
	unsigned char s;

	h = (str[0] - '0') * 10 + (str[1] - '0');
	m = (str[3] - '0') * 10 + (str[4] - '0');
	s = (str[6] - '0') * 10 + (str[7] - '0');

	if (h >= 24 || m >= 60 || s >= 60)
	{
		return false;
	}

	unsigned int sec = h * 3600 + m * 60 + s;
	unsigned int ttm = SECONDS_IN_DAY - sec;

	uint32_t t = timer_get_tick_count();
	_timer_nextmid = t;
	timer_add_seconds(&_timer_nextmid, ttm);

	_set = true;
	return true;
}

void time_get_time(char* str)
{
	if (!_set)
	{
		sprintf(str, "not set");
		return;
	}

	uint32_t t = timer_get_tick_count();

	while (timer_compare(_timer_nextmid, t) <= 0)
	{
		timer_add_seconds(&_timer_nextmid, SECONDS_IN_DAY);
	}

	unsigned int ttm = timer_get_diff_seconds(_timer_nextmid, t);
	unsigned int sec = SECONDS_IN_DAY - ttm;

	unsigned char h = (sec / 3600);
	unsigned char m = ((sec - h * 3600) / 60);
	unsigned char s = (sec % 60);

	str[0] = (h / 10) + '0';
	str[1] = h % 10 + '0';
	str[2] = ':';
	str[3] = (m / 10) + '0';
	str[4] = m % 10 + '0';
	str[5] = ':';
	str[6] = (s / 10) + '0';
	str[7] = s % 10 + '0';
	str[8] = 0;
}
