OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_vectors)
SECTIONS
{
    . = 0x00000000;

    __text_start = .;
    .text : {
        *(.text.startup)    
        *(.text)
        *(.text.*)
    }
    __text_end = .;

    __exidx_start = .;
    .ARM.exidx   : {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    }
    __exidx_end = .;

    __rodata_start = .;
    .rodata : {
        *(.rodata) *(.rodata.*)
    }
    __rodata_end = .;

    . = ALIGN(4);
    .ctors : {
        PROVIDE(__ctors_start__ = .);
        KEEP(*(SORT(.ctors.*)))
        KEEP(*(.ctors))
        PROVIDE(__ctors_end__ = .);
    }

    .dtors : {
        PROVIDE(__dtors_start__ = .);
        KEEP(*(SORT(.dtors.*)))
        KEEP(*(.dtors))
        PROVIDE(__dtors_end__ = .);
    }

    __data_start = .;
    . = ALIGN(4);
    .data : {
        *(.data)    
    }
    __data_end = .;

    . = ALIGN(4);
    __bss_start = __data_end;
    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)    
        . = ALIGN(4);
    }
    . = ALIGN(4);
    __bss_end = .;

    /* Stabs debugging sections.  */
    .stab 0 : { *(.stab) }
    .stabstr 0 : { *(.stabstr) }
    .stab.excl 0 : { *(.stab.excl) }
    .stab.exclstr 0 : { *(.stab.exclstr) }
    .stab.index 0 : { *(.stab.index) }
    .stab.indexstr 0 : { *(.stab.indexstr) }
    .comment 0 : { *(.comment) }

    __end = .;
}
