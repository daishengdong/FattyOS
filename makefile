CROSS_COMPILE=arm-none-eabi-
CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld
OBJCOPY=$(CROSS_COMPILE)objcopy
OBJDUMP=$(CROSS_COMPILE)objdump

TOP_DIR=$(shell pwd)
INCLUDE=-I$(TOP_DIR) -I$(TOP_DIR)/include

CFLAGS=-O2 -g -mthumb-interwork -mcpu=cortex-a15 -mfpu=vfpv3 -mfloat-abi=softfp $(INCLUDE)
ASFLAGS=-O2 -g -mthumb-interwork -mcpu=cortex-a15 -mfpu=vfpv3 -mfloat-abi=softfp $(INCLUDE)
LDFLAGS=-Tartos.ld -Ttext 60000000 -gc-sections

OBJS=start.o init.o exception.o application.o print.o klib.o trap.o interrupt.o gic.o timer.o printk.o

.c.o:
	$(CC) $(CFLAGS) -c $<

.S.o:
	$(CC) $(ASFLAGS) -c $<

artos: $(OBJS)
	echo $(TOP_DIR)
	$(CC) -static -nostartfiles -nostdlib $(LDFLAGS) -Wl,-Map=$@.map $? -o $@.elf -lgcc -Wl,--gc-sections
	$(OBJCOPY) -O binary $@.elf artos.bin
	$(OBJDUMP) -d $@.elf > $@.asm
	./qemu.sh

clean:
	rm *.o artos.map artos.elf artos.bin artos.asm sd.bin -f

.PHONY: artos
