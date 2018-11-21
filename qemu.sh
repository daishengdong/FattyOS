:<<comment
if [ ! -f "sd.bin" ]; then
dd if=/dev/zero of=sd.bin bs=1024 count=65536
fi
comment

# qemu-system-arm -M vexpress-a9 -kernel artos.elf -serial stdio -sd sd.bin
qemu-system-arm -M vexpress-a9 -kernel artos.elf -serial stdio
