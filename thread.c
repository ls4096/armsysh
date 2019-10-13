#include <string.h>
#include <stdint.h>

#include "thread.h"

#include "armsysh_constants.h"
#include "dump.h"
#include "pm.h"

static int _running = -1;
static uint32_t* _saved_sp;
static unsigned char _pipe_buf[THREAD_PIPE_BUF_SIZE];
static unsigned char _pipe_buf_next_write;
static unsigned char _pipe_buf_next_read;
static bool _pipe_in_end;
static unsigned int _thread_switch_count = 0;


static void thread_run(thread_entry_func func, void* arg);
static void return_from_thread();
static bool is_returned_from_thread();


bool thread_is_running()
{
	return (_running != -1);
}

int thread_which_is_running()
{
	return _running;
}

void thread_create(thread_entry_func func, void* arg)
{
	register uint32_t r asm("r4");
	asm volatile ("mov r4, sp");

	_saved_sp = (uint32_t*)r - (THREAD_STACK_OFFSET_BYTES >> 2);

	for (int i = 0; i < 13; i++)
	{
		switch (i)
		{
		case 0:
			*(_saved_sp--) = (uint32_t)func;
			break;
		case 1:
			*(_saved_sp--) = (uint32_t)arg;
			break;
		default:
			*(_saved_sp--) = 0;
			break;
		}
	}

	*(_saved_sp--) = (uint32_t)(&thread_run);

	asm volatile ("mrs r4, apsr");
	*_saved_sp = r;

	_pipe_buf_next_write = 0;
	_pipe_buf_next_read = 0;
	_pipe_in_end = false;
	_running = 0;
}

void thread_join()
{
	if (!thread_is_running())
	{
		dump_state();
	}

	_pipe_in_end = true;

	while (!is_returned_from_thread())
	{
		pm_yield();
	}

	_running = -1;
}

void thread_switch()
{
	asm volatile (
		"push	{r0}\r\n" \
		"push	{r1}\r\n" \
		"push	{r2}\r\n" \
		"push	{r3}\r\n" \
		"push	{r4}\r\n" \
		"push	{r5}\r\n" \
		"push	{r6}\r\n" \
		"push	{r7}\r\n" \
		"mov	r0, r8\r\n" \
		"push	{r0}\r\n" \
		"mov	r0, r9\r\n" \
		"push	{r0}\r\n" \
		"mov	r0, r10\r\n" \
		"push	{r0}\r\n" \
		"mov	r0, r11\r\n" \
		"push	{r0}\r\n" \
		"mov	r0, r12\r\n" \
		"push	{r0}\r\n" \
		"mov	r0, lr\r\n" \
		"push	{r0}\r\n" \
		"mrs	r0, apsr\r\n" \
		"push	{r0}\r\n"
	);

	register uint32_t* sp asm("r2");
	register uint32_t* spr asm("r3");
	asm volatile ("mov %0, %1" : "=r" (spr) : "r" (_saved_sp));
	asm volatile ("mov r2, sp");
	asm volatile ("mov sp, r3");
	asm volatile ("mov %0, %1" : "=r" (_saved_sp) : "r" (sp));

	if (_running != -2)
	{
		_running = (_running == 1 ? 0 : 1);
	}

	_thread_switch_count++;

	asm volatile (
		"pop	{r0}\r\n" \
		"msr	apsr, r0\r\n" \
		"pop	{r0}\r\n" \
		"mov	lr, r0\r\n" \
		"pop	{r0}\r\n" \
		"mov	r12, r0\r\n" \
		"pop	{r0}\r\n" \
		"mov	r11, r0\r\n" \
		"pop	{r0}\r\n" \
		"mov	r10, r0\r\n" \
		"pop	{r0}\r\n" \
		"mov	r9, r0\r\n" \
		"pop	{r0}\r\n" \
		"mov	r8, r0\r\n" \
		"pop	{r7}\r\n" \
		"pop	{r6}\r\n" \
		"pop	{r5}\r\n" \
		"pop	{r4}\r\n" \
		"pop	{r3}\r\n" \
		"pop	{r2}\r\n" \
		"pop	{r1}\r\n" \
		"pop	{r0}\r\n"
	);

	asm volatile ("bx lr");
}

unsigned int thread_switch_count()
{
	return _thread_switch_count;
}

bool thread_pipe_has_next_byte()
{
	return !(_pipe_buf_next_read == _pipe_buf_next_write);
}

unsigned char thread_read_pipe()
{
	while (!thread_pipe_has_next_byte())
	{
		if (_pipe_in_end)
		{
			// Pipe is empty and first thread is done.
			return 0x04;
		}

		pm_yield();
	}

	unsigned char c = _pipe_buf[_pipe_buf_next_read];
	_pipe_buf_next_read = ((_pipe_buf_next_read + 1) % THREAD_PIPE_BUF_SIZE);

	return c;
}

bool thread_is_pipe_full()
{
	return (((_pipe_buf_next_write + 1) % THREAD_PIPE_BUF_SIZE) == _pipe_buf_next_read);
}

void thread_write_pipe(unsigned char c)
{
	if (is_returned_from_thread())
	{
		return;
	}

	while (thread_is_pipe_full())
	{
		pm_yield();
	}

	_pipe_buf[_pipe_buf_next_write] = c;
	_pipe_buf_next_write = ((_pipe_buf_next_write + 1) % THREAD_PIPE_BUF_SIZE);
}


static void thread_run(thread_entry_func func, void* arg)
{
	func(arg);
	return_from_thread();
}

static void return_from_thread()
{
	if (_running != 1)
	{
		dump_state();
	}

	_running = -2;
	thread_switch();
}

static bool is_returned_from_thread()
{
	return (_running == -2);
}
