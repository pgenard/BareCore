ENTRY(_start)

MEMORY
{
    RAM (rwx) : ORIGIN = 0x40000000, LENGTH = 512M
}

/*
 * Separate ELF load segments:
 *  PF_R = 4
 *  PF_W = 2
 *  PF_X = 1
 */
PHDRS
{
    text   PT_LOAD FLAGS(5);  /* Read + Execute */
    rodata PT_LOAD FLAGS(4);  /* Read only */
    data   PT_LOAD FLAGS(6);  /* Read + Write */
}

SECTIONS
{
    . = ORIGIN(RAM);

    /* Executable code */
    .text.boot :
    {
        KEEP(*(.text.boot))
    } > RAM :text

    . = ALIGN(2048);

    .vectors :
    {
        KEEP(*(.vectors))
    } > RAM :text

    .text :
    {
        *(.text*)
    } > RAM :text

    /* Read-only data */
    . = ALIGN(4096);

    .rodata :
    {
        *(.rodata*)
    } > RAM :rodata

    /* Initialized writable data */
    . = ALIGN(4096);

    .data :
    {
        __sdata = .;
        *(.data*)
        __edata = .;
    } > RAM :data

    __sidata = LOADADDR(.data);

    /* Zero-initialized data */
    .bss :
    {
        . = ALIGN(16);
        __sbss = .;

        *(.bss*)
        *(COMMON)

        . = ALIGN(16);
        __ebss = .;
    } > RAM :data

    /* Stack */
    . = ALIGN(16);
    . += 16384;

    __stack_top = .;
}
