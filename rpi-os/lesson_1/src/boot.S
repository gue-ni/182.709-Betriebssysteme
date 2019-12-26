#include "mm.h"

.section ".text.boot"

.globl _start
_start:
	mrs	x0, mpidr_el1	// load processor id to register x0	
	and	x0, x0,#0xFF	// check processor id
	cbz	x0, master		// branch to master is 0x is 0
	b	proc_hang		// branch to label proc_hand

proc_hang: 
	b 	proc_hang

master:
	adr	x0, bss_begin
	adr	x1, bss_end
	sub	x1, x1, x0
	bl 	memzero

	mov	sp, #LOW_MEMORY 
	bl	kernel_main 	// call kernel_main() in kernel.c
	b 	proc_hang		// should never come here
