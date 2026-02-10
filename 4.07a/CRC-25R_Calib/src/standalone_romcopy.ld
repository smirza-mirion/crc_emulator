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
// Program layout for starting in ROM, copying text and data to RAM,
// and then running out of RAM.
//

SECTIONS
{
//
// RAM SECTIONS
//
    .boottext ALIGN(4) : {
	  // All .text which is reachable between
	  // _start and __ghs_ind_crt1 needs to be
	  // pulled into .boottext and left
	  // uncompressed.
	  crt0.o(.text)
	  libstartup.a(*)(.text)
	  libsys.a(ind_crt1.o)(.text)
	  libarch.a(*)(.text)
    }							      > dram_memory
    .syscall					   ALIGN(4) : > .
    .text					   ALIGN(4) : > .

    .data						    : > .
    .bss						    : > .

    .sdabase			                   ALIGN(4) : > .
    .sdata					            : > .
    .sbss						    : > .

    .heap	                 ALIGN(8) PAD(heap_reserve) : > .

    .stack                  ALIGN(4) PAD(stack_reserve) ABS : > .
    .zdata			                        ABS : > zero_memory
    .zbss					        ABS : > .

//
// ROM SECTIONS
//

    .rodata						    : > flash_memory

    .ROM.boottext		    ALIGN(4) ROM(.boottext) : > .
    .ROM.syscall		     ALIGN(4) ROM(.syscall) : > .
    .secinfo						    : > .
    .fixaddr						    : > .
    .fixtype						    : > .

    .CROM.zdata				       CROM(.zdata) : > .
    .CROM.data				        CROM(.data) : > .
    .CROM.sdata				       CROM(.sdata) : > .
    .CROM.text			                CROM(.text) : > .

//
// These special symbols mark the bounds of RAM and ROM images of boot code.
// They are used by the GHS startup code (_start and __ghs_ind_crt0).
//
    __ghs_rambootcodestart  = ADDR(.boottext);
    __ghs_rambootcodeend    = ENDADDR(.text);	
    __ghs_rombootcodestart  = ADDR(.ROM.boottext);
    __ghs_rombootcodeend    = ENDADDR(.fixtype);

//
// These special symbols mark the bounds of RAM and ROM memory.
// They are used by the MULTI debugger.
//
    __ghs_ramstart  = MEMADDR(dram_memory);
    __ghs_ramend    = MEMENDADDR(dram_memory);
    __ghs_romstart  = MEMADDR(flash_rsvd1);
    __ghs_romend    = MEMENDADDR(flash_memory);

//
// This special symbol marks the the first address executed after the
// ROM to RAM copy is complete.  It is used by the MULTI debugger.
//
    __ghs_after_romcopy = __ghs_ind_crt1;
}






