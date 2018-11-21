OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS
{
    
    . = 0x00000000;
    .text : {
        *(.text.startup)    
        *(.text)
    }

    .rodata ALIGN(32) : {
        *(.rodata)
    }
    . = ALIGN(4);

    .ctors : {
        PROVIDE(__ctors_start__ = .);
        KEEP(*(SORT(.ctors.*)))
        KEEP(*(.ctors))
        PROVIDE(__ctors_end__ = .);
    }

    . = ALIGN(4);
    .dtors : {
        PROVIDE(__dtors_start__ = .);
        KEEP(*(SORT(.dtors.*)))
        KEEP(*(.dtors))
        PROVIDE(__dtors_end__ = .);
    }

    .data ALIGN(32) : {
        *(.data)    
    }

    __bss_start = .;
    .bss ALIGN(32) : {
        *(.bss)    
        *(COMMON)    
    }
    __bss_end = .;

    /DISCARD/ : {
        *(.note.*)
    }
    __end = .;
}
