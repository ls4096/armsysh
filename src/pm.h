#ifndef _PM_H_
#define _PM_H_

void pm_reset();
void pm_yield();

void pm_update_wake_counter(unsigned int c);
void pm_get_wake_count(unsigned int w[2]);

#endif // _PM_H_
