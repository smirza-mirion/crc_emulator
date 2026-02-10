MEMORY{
	zero_memory					: ORIGIN = 0x00000000, LENGTH = 0K
	dram_memory					: ORIGIN = 0x02000000, LENGTH = 16256K
	stack_memory				: ORIGIN = .,		   LENGTH = 64K
	ethernet_transmit_memory	: ORIGIN = 0x02FF0000,		   LENGTH = 32K
	ethernet_receive_memory		: ORIGIN = 0x02FF8000,		   LENGTH = 32K
	vector_memory				: ORIGIN = 0x20000000, LENGTH = 1K
	sram_memory					: ORIGIN = 0x20000800, LENGTH = 62K //64K
	cf_reg_memory				: ORIGIN = 0x40000000, LENGTH = 2M
	flash_rsvd1					: ORIGIN = 0xFFE00000, LENGTH = 256K
	flash_memory				: ORIGIN = .,          LENGTH = 2M-256K
	// 	flash_image					: ORIGIN = 0x02700000, LENGTH = 1M
}

CONSTANTS{
	stack_reserve = 64K 
}

//
// Program layout for running out of RAM.
//
//

SECTIONS{
// Place the zero data area in low memory so we can use the 16-bit
// absolute addressing mode
	.zdata					ABS : > zero_memory
	.zbss					ABS : > .

// Code
	.text					ALIGN(4) : > dram_memory

// The syscall section is a special section that allows communication
// between the application and the MULTI debugger and/or simulator
	.syscall                            : > .
    
// Non-zero-initialized data
	.data								: > .
	.secinfo							: > .

// Zero-initialized data
	.bss								: > .
    
// Read-only data
	.rodata								: > .

// The following two sections reserve space for the heap and the stack.
	.heap					ALIGN(8) PAD(addr(.stack) - absolute(.)) : > .
	.stack					PAD(stack_reserve) : > stack_memory

//add section for ethernet transmit buffer
	ethertx					ABS : > ethernet_transmit_memory

//add section for ethernet receive buffer
	etherrx					ABS : > ethernet_receive_memory

//add vector section
	vectsect				ABS : > vector_memory

//add section for saved data -- in SRAM
//	savesect				ABS : > sram_memory

//add cf section
	cfsect					ABS : > cf_reg_memory

//add flash image section
//    imagesect				ABS : > flash_image

// The .secinfo section contains information about how to initialize
// memory when your application loads.  It tells us which sections of
// RAM need to be zero-initialized and which sections need to be copied
// from ROM.
// The .fixaddr and .fixtype section contain information needed for PIC and
// PID modes.
	.fixaddr							: > .
    .fixtype							: > .

// These special symbols mark the bounds of RAM and ROM memory.
// They are used by the MULTI debugger.
	__ghs_ramstart  = MEMSIZEOF(zero_memory) ? MEMADDR(zero_memory) : MEMADDR(dram_memory);
	__ghs_ramend    = MEMENDADDR(dram_memory);
	__ghs_romstart  = MEMADDR(flash_rsvd1);
	__ghs_romend    = MEMENDADDR(flash_memory);

//  Clive: added to setup SP,PC in crt0.68
//	__ghs_endstack  = ENDADDR(.stack);

//	__ghsbegin_heap  = ADDR(.heap);
}
