ENTRY(start)

SECTIONS
{
    . = 1M;

    .boot :
    {
        KEEP(*(.multiboot_header))
    }

    . += 0xFFFFFFFF80000000;

    _kernel_start = .;

    .text : AT(ADDR(.text) - 0xFFFFFFFF80000000)
    {
        *(.text)
    }

    .rodata : AT(ADDR(.rodata) - 0xFFFFFFFF80000000)
    {
        *(.rodata)
    }

    .data : AT(ADDR(.data) - 0xFFFFFFFF80000000)
    {
        *(.data)
    }

    .bss : AT(ADDR(.bss) - 0xFFFFFFFF80000000)
    {
        *(.bss)
        *(COMMON)
    }

    _kernel_end = .;
}