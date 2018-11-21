# FattyOS
- 这是出于个人正在着手开发的RTOS内核。

## 开发环境
- 选择了qemu作为开发环境，因为qemu可以结合gdb进行调试，同时可以摆脱对具体硬件的依赖。
- 硬件选型vexpress-a9。其实本意是想选择qemu中的virt作为模拟硬件，因为magenta有完善的qemu下virt环境的支持。可是网上很少有资料关于virt硬件模拟方面的描述。关于qemu virt模拟方面的文档资料：
- [qemu官方doc](https://wiki.qemu.org/Documentation/Platforms/ARM)
- [virt上运行debian](https://translatedcode.wordpress.com/2016/11/03/installing-debian-on-qemus-32-bit-arm-virt-board/)
- 很可惜，上面的文档并不完善。权衡之下选择了vexpress-a9这个模拟硬件，这里主要基于的考虑是rtthread有对qemu下vexpress-a9较为完善的支持，后续遇到困难从rtthread上获取驱动等也不愧为一个捷径，同时该模拟硬件可以提供很好的对多核的模拟，对后续实践SMP等也是很好的。

## 目的
- 现在开源的RTOS内核不知几千几万，本人重复造轮子从零开始写自己的内核，主要还是旨在以实践贯彻知识，并没有什么明确的目的，纯粹是个人兴趣，后续如果精力允许，基于此内核移植开发网络协议栈，开发自己的编译器、解释器、动态链接器，以及深入研究并实践SMP、微内核等机制。

## 环境搭建
- [开发环境搭建](https://www.jianshu.com/p/72b0123e0c46)

## 如何运行
- 在主目录下直接make即可，make主要分为两步，先编译生成内核，再调用qemu.sh运行内核
- 准备用scons作为构建工具的，奈何scons还没学会，暂时先用make吧（其实make也没精通）
- 如果想调试的话，在qemu.sh后添加-S -s或者 -gdb tcp::1234 -S参数启动内核。同时打开另外一个终端，输入arm-none-eabi-gdb，在gdb中file artos.elf（没错，FattyOS本意是想取名aRTOS的，但我github中已有ArtOS，无法再创建该仓，故而命名为FattyOS），再target remote:1234即可连上。注意这里一定要用arm-none-eabi-gdb而不是你主机（x86）上的那个gdb，否则无法调试（网上很多资料比较误人子弟，写的是gdb）
