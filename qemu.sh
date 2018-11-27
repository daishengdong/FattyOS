debug=$1

:<<comment
if [ ! -f "sd.bin" ]; then
dd if=/dev/zero of=sd.bin bs=1024 count=65536
fi
comment

# qemu-system-arm -M vexpress-a9 -kernel artos.elf -serial stdio -sd sd.bin
if [ "$debug"x = "y"x ]; then
    qemu-system-arm -M vexpress-a9 -kernel artos.elf -serial stdio -S -s
else
    qemu-system-arm -M vexpress-a9 -kernel artos.elf -serial stdio
fi
