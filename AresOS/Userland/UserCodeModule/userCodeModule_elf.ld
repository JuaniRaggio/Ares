OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)
SECTIONS
{
	.text 0x400000 :
	{
		*(.text*)
		. = ALIGN(0x1000);
		*(.rodata*)
	}
	.data ALIGN(0x1000) :
	{
		*(.data*)
	}
	.bss ALIGN(0x1000) :
	{
		bss = .;
		*(.bss*)
	}
	endOfBinary = .;
}
