ifndef ARMSYSH_SERIAL_BAUDRATE
	ARMSYSH_SERIAL_BAUDRATE := "115200"
endif

all: armsysh.bin

OBJS = \
	src/startup_sam3xa.o \
	src/system_sam3xa.o \
	src/main.o \
	src/bricks.o \
	src/command.o \
	src/draw.o \
	src/dump.o \
	src/grep.o \
	src/handler.o \
	src/lib.o \
	src/pm.o \
	src/pong.o \
	src/reset.o \
	src/rng.o \
	src/seq.o \
	src/serial.o \
	src/serial_proxy.o \
	src/snake.o \
	src/sp_mon.o \
	src/term.o \
	src/thread.o \
	src/time.o \
	src/timer.o \
	src/util.o \
	src/wc.o

%.o: %.c
	arm-none-eabi-gcc -c -Os -mcpu=cortex-m3 -D__SAM3X8E__ -DBAUDRATE=$(ARMSYSH_SERIAL_BAUDRATE) -I$(ARMSYSH_CMSIS_DIR)/Device/ATMEL/sam3xa/include/ -I$(ARMSYSH_CMSIS_DIR)/CMSIS/Include/ -o $@ $<

%.o: %.S
	arm-none-eabi-as -c -mcpu=cortex-m3 -o $@ $<

armsysh.elf: $(OBJS)
	arm-none-eabi-ld -Os -T sam3x8e_flash.ld -o armsysh.elf $(OBJS)
	arm-none-eabi-size armsysh.elf

armsysh.bin: armsysh.elf
	arm-none-eabi-objcopy -O binary armsysh.elf armsysh.bin

clean:
	rm -rf src/*.o *.elf *.bin

flash: armsysh.bin
	bossac -p $(ARMSYSH_FLASH_PORT) -e -w -v -b armsysh.bin
