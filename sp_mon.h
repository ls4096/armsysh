#ifndef _SP_MON_H_
#define _SP_MON_H_

#include <stdint.h>

#define SP_MON_BUCKET_SIZE_BITS 8 // Buckets of 256 bytes.
#define SP_MON_NUM_BUCKETS (0x8000 >> SP_MON_BUCKET_SIZE_BITS) // Assumes 32K stack.

#include <stdbool.h>


void sp_mon_enable(bool enable);

void sp_mon_check();

unsigned int* sp_mon_get_buckets();


#endif // _SP_MON_H_
