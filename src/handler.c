#include <stdio.h>
#include <string.h>

#include "handler.h"

#include "serial.h"
#include "dump.h"

#include <sam3x8e.h>

void handle_missing(void)
{
	char buf[64];

	sprintf(buf, "Unhandled exception!\r\n");
	serial_write(buf, strlen(buf));

	sprintf(buf, "SCB_ICSR: %08x\r\n", SCB->ICSR);
	serial_write(buf, strlen(buf));

	sprintf(buf, "Dumping more state...\r\n");
	serial_write(buf, strlen(buf));

	dump_state();
}
