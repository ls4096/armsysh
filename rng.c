#include "rng.h"

// A simple 32-bit LCG implementation that allows for adding optional entropy.

// Constants chosen to satisfy Hull-Dobell requirements (with implied m=2^32).
#define A 36777773
#define C 178666669

static volatile unsigned int _r = 81618211;

void rng_add_entropy(unsigned char e)
{
	_r ^= e;
}

int rng_rand()
{
	_r = A * _r + C;
	return (int)_r;
}
