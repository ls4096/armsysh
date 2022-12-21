.global dump_state
dump_state:
	cpsid	f

	push	{lr}
	push	{r0}
	push	{r1}
	push	{r2}
	push	{r3}
	push	{r4}
	push	{r5}
	push	{r6}
	push	{r7}
	mov	r0, r8
	push	{r0}
	mov	r0, r9
	push	{r0}
	mov	r0, r10
	push	{r0}
	mov	r0, r11
	push	{r0}
	mov	r0, r12
	push	{r0}

	mov	r4, #13

	mov	r7, #1
	mov	r6, #2

	mov	r1, #0x30

.print_reg_loop:
	/* Loop through register values */

	mov	r0, #'R'
	bl	serial_tx_byte

	add	r0, r7, r1
	bl	serial_tx_byte

	add	r0, r6, r1
	bl	serial_tx_byte

	mov	r0, #':'
	bl	serial_tx_byte

	mov	r0, #'\t'
	bl	serial_tx_byte

	pop	{r0}
	bl	_print_4byte_hex

	bl	_print_nl

	cmp	r6, #0
	bne	.prl0
	mov	r6, #10
	sub	r7, #1
.prl0:
	sub	r6, #1
	sub	r4, #1
	cmp	r4, #0
	bne	.print_reg_loop

	/* LR from stack to r7 */
	pop	{r7}

	/* SP to r12 */
	mov	r12, sp

	mov	r0, #'S'
	bl	serial_tx_byte
	mov	r0, #'P'
	bl	serial_tx_byte
	mov	r0, #':'
	bl	serial_tx_byte
	mov	r0, #'\t'
	bl	serial_tx_byte
	mov	r0, r12
	bl	_print_4byte_hex

	bl	_print_nl

	mov	r0, #'L'
	bl	serial_tx_byte
	mov	r0, #'R'
	bl	serial_tx_byte
	mov	r0, #':'
	bl	serial_tx_byte
	mov	r0, #'\t'
	bl	serial_tx_byte
	mov	r0, r7
	bl	_print_4byte_hex

	bl	_print_nl


	/* Print some stack bytes */
	mov	r1, #8

.print_stack_loop:
	mov	r0, #' '
	bl	serial_tx_byte

	pop	{r0}
	bl	_print_4byte_hex
	bl	_print_nl

	sub	r1, #1
	cmp	r1, #0
	bne	.print_stack_loop


.ds_sleep:
	wfi
	b	.ds_sleep


_print_nl:
	push	{r0, lr}
	mov	r0, #0x0d
	bl	serial_tx_byte
	mov	r0, #0x0a
	bl	serial_tx_byte
	pop	{r0, pc}


_print_4byte_hex:
	push	{r0, r1, lr}
	mov	r1, #8
	rev	r0, r0
	bl	_print_byte_hex
	ror	r0, r0, r1
	bl	_print_byte_hex
	ror	r0, r0, r1
	bl	_print_byte_hex
	ror	r0, r0, r1
	bl	_print_byte_hex
	pop	{r0, r1, pc}


_print_byte_hex:
	push	{r4, r7, lr}
	push	{r0}

	mov	r7, #4
	mov	r4, #0x0f

	ror	r0, r0, r7
	and	r0, r4
	cmp	r0, #0x0a
	blo	.pbh0
	add	r0, #0x27
.pbh0:
	add	r0, #0x30
	bl	serial_tx_byte
	pop	{r0}

	push	{r0}
	and	r0, r4
	cmp	r0, #0x0a
	blo	.pbh1
	add	r0, #0x27
.pbh1:
	add	r0, #0x30
	bl	serial_tx_byte

	pop	{r0}
	pop	{r4, r7, pc}
