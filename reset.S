.global reset
reset:
	mov	r1, #0x08
	lsl	r1, #16
	ldr	r2, [r1]
	mov	sp, r2
	b	Reset_Handler
