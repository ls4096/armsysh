#include "serial.h"

#include "armsysh_constants.h"
#include "pm.h"
#include "thread.h"
#include "timer.h"

#include <sam3x8e.h>

#ifndef BAUDRATE
	#define BAUDRATE 115200
#endif

static const unsigned char NEWLINE[2] = { 0x0d, 0x0a };


static volatile unsigned char _rx_buf[SERIAL_RX_BUF_SIZE];
static volatile unsigned char _rx_buf_next_read = 0;
static volatile unsigned char _rx_buf_next_write = 0;

static volatile unsigned char _tx_buf[SERIAL_RX_BUF_SIZE];
static volatile unsigned char _tx_buf_next_read = 0;
static volatile unsigned char _tx_buf_next_write = 0;


static void serial_init_hw();


void UART_Handler()
{
	if ((UART->UART_SR & UART_SR_RXRDY) != UART_SR_RXRDY)
	{
		return;
	}

	unsigned char c = UART->UART_RHR;
	if (((_rx_buf_next_write + 1) % SERIAL_RX_BUF_SIZE) == _rx_buf_next_read)
	{
		return;
	}

	_rx_buf[_rx_buf_next_write] = c;
	_rx_buf_next_write = ((_rx_buf_next_write + 1) % SERIAL_RX_BUF_SIZE);
}


void serial_init()
{
	serial_init_hw();
}

bool serial_has_next_byte()
{
	return !(_rx_buf_next_read == _rx_buf_next_write);
}

unsigned char serial_read_next_byte()
{
	if (thread_is_running() && thread_which_is_running() == 1)
	{
		return thread_read_pipe();
	}

	while (!serial_has_next_byte())
	{
		pm_yield();
	}

	unsigned char c = _rx_buf[_rx_buf_next_read];
	_rx_buf_next_read = ((_rx_buf_next_read + 1) % SERIAL_RX_BUF_SIZE);

	return c;
}

void serial_write(const unsigned char* data, int len)
{
	int i;
	for (i = 0; i < len; i++)
	{
		serial_tx_byte(data[i]);
	}
}

void serial_write_newline()
{
	serial_write(NEWLINE, sizeof(NEWLINE));
}

void serial_tx_byte(unsigned char data)
{
	if (thread_is_running() && thread_which_is_running() == 0)
	{
		thread_write_pipe(data);
	}
	else
	{
		while ((UART->UART_SR & UART_SR_TXRDY) != UART_SR_TXRDY) { }
		UART->UART_THR = data;
	}
}

static void serial_init_hw()
{
	PIOA->PIO_PDR = PIO_PA8A_URXD | PIO_PA9A_UTXD;

	PMC->PMC_PCER0 |= PMC_PCER0_PID8;

	UART->UART_PTCR = UART_PTCR_RXTDIS | UART_PTCR_TXTDIS;
	UART->UART_CR = UART_CR_RSTRX | UART_CR_RSTTX | UART_CR_RXDIS | UART_CR_TXDIS;
	UART->UART_MR = UART_MR_CHMODE_NORMAL | UART_MR_PAR_NO;
	UART->UART_BRGR = (SystemCoreClock / BAUDRATE) >> 4;

	UART->UART_IER = UART_IER_RXRDY;

	NVIC_EnableIRQ(UART_IRQn);

	UART->UART_CR = UART_CR_RXEN | UART_CR_TXEN;
}

unsigned int serial_get_baudrate()
{
	return BAUDRATE;
}
