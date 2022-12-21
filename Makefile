ifndef ARMSYSH_SERIAL_BAUDRATE
	ARMSYSH_SERIAL_BAUDRATE := "115200"
endif

all: armsysh.bin

OBJS = \
	bricks.o \
	command.o \
	draw.o \
	dump.o \
	grep.o \
	lib.o \
	main.o \
	pm.o \
	pong.o \
	reset.o \
	rng.o \
	seq.o \
	serial.o \
	serial_proxy.o \
	snake.o \
	sp_mon.o \
	term.o \
	thread.o \
	time.o \
	timer.o \
	util.o \
	wc.o \
	startup_sam3xa.o \
	system_sam3xa.o

%.o: %.c
	arm-none-eabi-gcc -c -Os -mcpu=cortex-m3 -mthumb -D__SAM3X8E__ -DBAUDRATE=$(ARMSYSH_SERIAL_BAUDRATE) -I$(ARMSYSH_CMSIS_DIR)/Device/ATMEL/sam3xa/include/ -I$(ARMSYSH_CMSIS_DIR)/CMSIS/Include/ -o $@ $<

%.o: %.S
	arm-none-eabi-as -c -mcpu=cortex-m3 -mthumb -o $@ $<

armsysh.elf: $(OBJS)
	arm-none-eabi-ld -Os -T sam3x8e_flash.ld -o armsysh.elf *.o
	arm-none-eabi-size armsysh.elf

armsysh.bin: armsysh.elf
	arm-none-eabi-objcopy -O binary armsysh.elf armsysh.bin

clean:
	rm -rf *.o *.elf *.bin

flash: armsysh.bin
	bossac -p $(ARMSYSH_FLASH_PORT) -e -w -v -b armsysh.bin
