#ifndef _GAME_H_
#define _GAME_H_

#include "pm.h"
#include "serial.h"
#include "timer.h"


typedef struct
{
	uint32_t next_frame_time;
	unsigned int game_frame_ticks;
} game_context_t;


static void game_context_init(game_context_t* ctx, unsigned int game_fps);
static bool game_should_draw_frame(game_context_t* ctx);
static unsigned char game_get_char(game_context_t* ctx, bool block);
static void game_add_frame_time(game_context_t* ctx);


static void game_context_init(game_context_t* ctx, unsigned int game_fps)
{
	ctx->next_frame_time = 0;
	ctx->game_frame_ticks = TIMER_TICKS_PER_SECOND / game_fps;
}

static bool game_should_draw_frame(game_context_t* ctx)
{
	if (ctx->next_frame_time == 0)
	{
		ctx->next_frame_time = timer_get_tick_count();
		game_add_frame_time(ctx);
		return true;
	}

	uint32_t t = timer_get_tick_count();
	if (timer_compare(t, ctx->next_frame_time) >= 0)
	{
		ctx->next_frame_time = t;
		game_add_frame_time(ctx);
		return true;
	}

	return false;
}

static unsigned char game_get_char(game_context_t* ctx, bool block)
{
	if (!block)
	{
		uint32_t t = timer_get_tick_count();
		if (timer_compare(t, ctx->next_frame_time) >= 0)
		{
			return 0;
		}
		else
		{
			timer_notify_t tn;

			tn.t = ctx->next_frame_time;
			tn.notify = false;

			timer_notify_register(&tn);
			while (!tn.notify)
			{
				pm_yield();

				if (serial_has_next_byte())
				{
					return serial_read_next_byte();
				}
			}

			return 0;
		}
	}

	return serial_read_next_byte();
}

static void game_add_frame_time(game_context_t* ctx)
{
	ctx->next_frame_time += ctx->game_frame_ticks;
}

#endif // _GAME_H_
