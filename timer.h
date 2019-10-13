#ifndef _TIMER_H_
#define _TIMER_H_

#include <stdint.h>

#define TIMER_TICKS_PER_SECOND 250


void timer_init();
uint32_t timer_get_tick_count();
unsigned char timer_get_tick_count_lsbyte();
int timer_compare(uint32_t t0, uint32_t t1);
void timer_add_seconds(uint32_t* t, unsigned int seconds);
unsigned int timer_get_diff_seconds(uint32_t t0, uint32_t t1);


typedef struct {
	uint32_t t;
	bool notify;
} timer_notify_t;

bool timer_notify_register(timer_notify_t* tn);
unsigned int timer_get_notify_registered_count();


#endif // _TIMER_H_
