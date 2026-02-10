//	68K/ColdFire ELF default.ld

//	Consult the Green Hills Development Guide (Runtime Environment chapter)
//	for a description of the various sections in the section map.

SECTIONS
{
    .zdata 0xffff8000      	    ABS :
    .zbss                  	    ABS :
    .stack 0xffd00000	pad(0x200000)	:
    .text		0x00010000	:    
    .syscall				:
    .rodata				:
    .secinfo				:
    .sdabase align(4)			:
    .sdata				:
    .sbss				:    
    .data 				:
    .bss				:
    .heap align(16)	pad(0x400000)	:
}
