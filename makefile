# -include ./build-magenta-qemu-arm32/config-global.h
# --include ./build-magenta-qemu-arm32/arch/arm/config-module.h
# -include ./build-magenta-qemu-arm32/config-kernel.h

CROSS_COMPILE=arm-linux-gnueabi-
CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld
OBJCOPY=$(CROSS_COMPILE)objcopy
OBJDUMP=$(CROSS_COMPILE)objdump

CFLAGS=-O2 -g -mthumb-interwork -mcpu=cortex-a15 -mfpu=vfpv3 -mfloat-abi=softfp
ASFLAGS=-O2 -g -mthumb-interwork -mcpu=cortex-a15 -mfpu=vfpv3 -mfloat-abi=softfp
LDFLAGS=-Tartos.ld -Ttext 60000000 -gc-sections

OBJS=start.o init.o exception.o application.o print.o

.c.o:
	$(CC) $(CFLAGS) -c $<

.S.o:
	$(CC) $(ASFLAGS) -c $<

artos: $(OBJS)
	$(CC) -static -nostartfiles -nostdlib $(LDFLAGS) -Wl,-Map=$@.map $? -o $@.elf -lgcc -Wl,--gc-sections
	$(OBJCOPY) -O binary $@.elf artos.bin
	$(OBJDUMP) -d $@.elf > $@.asm
	./qemu.sh

clean:
	rm *.o artos.map artos.bin artos.asm -f

.PHONY: artos
