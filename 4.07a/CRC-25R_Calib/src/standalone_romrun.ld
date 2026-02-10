MEMORY {
        zero_memory : ORIGIN = 0x00000000, LENGTH = 0K
	vector_memory : ORIGIN = 0x0, LENGTH = 1K	
        dram_memory : ORIGIN = 0x00020000, LENGTH = 8M-128K
	sram_memory : ORIGIN = 0x20000000, LENGTH = 64K
	flash_rsvd1 : ORIGIN = 0xFFE00000, LENGTH = 256K
	flash_memory: ORIGIN = .,          LENGTH = 2M-256K
}

CONSTANTS {
    stack_reserve = 32K 
    heap_reserve = 512K 
}

//
// Program layout for starting in ROM, copying data to RAM,
// and continuing to execute out of ROM.
//

SECTIONS
{

//
// RAM SECTIONS
//
    .zdata                           ABS  : > zero_memory
    .zbss               	     ABS  : > .

// Small data area:
    .sdabase            	ALIGN(4)  : > dram_memory
    .sdata              	          : > .
    .sbss               	          : > .

// Non-zero-initialized data
    .data               	          : > .
// Zero-initialized data
    .bss                	          : > .

// The following two sections reserve space for the heap and the stack.
    .heap 	       PAD(heap_reserve)  : > .
    .stack 	       PAD(stack_reserve) : > .

    .ROM.zdata	              ROM(.zdata) : > flash_memory
    .ROM.sdata                ROM(.sdata) : > .
    .ROM.data       	       ROM(.data) : > .

// Code
    .text               	ALIGN(4)  : > .

// The syscall section is a special section that allows communication
// between the application and the MULTI debugger and/or simulator
    .syscall                    ALIGN(4)  : > .

// Read-only data
    .rodata             	          : > .

// The .secinfo section contains information about how to initialize
// memory when your application loads.  It tells us which sections of
// RAM need to be zero-initialized and which sections need to be copied
// from ROM.
// The .fixaddr and .fixtype section contain information needed for PIC and
// PID modes.
    .secinfo                              : > .
    .fixaddr                              : > .
    .fixtype                              : > .


//
// These special symbols mark the bounds of RAM and ROM memory.
// They are used by the MULTI debugger.
//
    __ghs_ramstart  = MEMSIZEOF(zero_memory) ? MEMADDR(zero_memory) : MEMADDR(dram_memory);
    __ghs_ramend    = MEMENDADDR(dram_memory);
    __ghs_romstart  = MEMADDR(flash_rsvd1);
    __ghs_romend    = MEMENDADDR(flash_memory);

//
// These special symbols mark the bounds of RAM and ROM images of boot code.
// They are used by the GHS startup code (_start and __ghs_ind_crt0).
//
    __ghs_rambootcodestart  = 0;
    __ghs_rambootcodeend    = 0;
    __ghs_rombootcodestart =  ADDR(.text);
    __ghs_rombootcodeend =    ENDADDR(.fixtype);

}

