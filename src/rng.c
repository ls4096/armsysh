#include "rng.h"

#include <sam3x8e.h>

// A simple 32-bit LCG implementation that allows for updating using additional entropy from the TRNG.

// Constants chosen to satisfy Hull-Dobell requirements (with implied m=2^32).
#define A 36777773
#define C 178666669

static volatile unsigned int _r = 81618211;

void rng_init()
{
	PMC->PMC_PCER1 |= PMC_PCER1_PID41;
	TRNG->TRNG_CR = (0x524e47 << 8) | 0x01;
}

void rng_update()
{
	_r ^= TRNG->TRNG_ODATA;
}

int rng_rand()
{
	_r = A * _r + C;
	return (int)_r;
}
