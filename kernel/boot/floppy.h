/******************************************************************************/
/* Important Spring 2016 CSCI 402 usage information:                          */
/*                                                                            */
/* This fils is part of CSCI 402 kernel programming assignments at USC.       */
/*         53616c7465645f5f2e8d450c0c5851acd538befe33744efca0f1c4f9fb5f       */
/*         3c8feabc561a99e53d4d21951738da923cd1c7bbd11b30a1afb11172f80b       */
/*         984b1acfbbf8fae6ea57e0583d2610a618379293cb1de8e1e9d07e6287e8       */
/*         de7e82f3d48866aa2009b599e92c852f7dbf7a6e573f1c7228ca34b9f368       */
/*         faaef0c0fcf294cb                                                   */
/* Please understand that you are NOT permitted to distribute or publically   */
/*         display a copy of this file (or ANY PART of it) for any reason.    */
/* If anyone (including your prospective employer) asks you to post the code, */
/*         you must inform them that you do NOT have permissions to do so.    */
/* You are also NOT permitted to remove or alter this comment block.          */
/* If this comment block is removed or altered in a submitted file, 20 points */
/*         will be deducted.                                                  */
/******************************************************************************/

#pragma once

segment_inc:
		.word	0x1000

bytes_per_sector:
		.word	512
sectors_per_track:
		.word	0
heads_per_cylinder:
		.word	0

absolute_sector:
		.byte	0x00
absolute_head:
		.byte	0x00
absolute_track:
		.byte	0x00

dot_string:
		.string "."
newline_string:
		.string "\n\r"
failed_string:
		.string "failed to read disk"

disk_error:
		mov     $failed_string, %si
		call    puts16
		hlt
		
		 /* Read disk geometry into global variables
		  * DL=>drive index
		  *
		  * uses interrupt 0x13 function 0x08 */
.read_disk_geometry:
		push	%ax
		push	%bx
		push	%cx
		push	%dx
		push	%es
		push	%di

		/* set es:di to 0x0:0x0 to handle buggy BIOS */
		mov		$0x00, %di
		mov		%di, %es

		mov		$0x08, %ah
		int		$0x13
		jc		disk_error

		inc		%dh
		mov		%dh, heads_per_cylinder
		
		mov		%cl, %ah
		and		$0x3f, %ah
		mov		%ah, sectors_per_track
		
		pop		%di
		pop		%es
		pop		%dx
		pop		%cx
		pop		%bx
		pop		%ax

		ret
		
		/* Convert LBA to CHS
		 * AX=>LBA Address to convert
		 *
		 * absolute sector = (logical sector / sectors per track) + 1
		 * absolute head   = (logical sector / sectors per track) MOD number of heads
		 * absolute track  = logical sector / (sectors per track * number of heads) */
.lba_to_chs:
		push	%dx
		
		xor		%dx, %dx
		divw	sectors_per_track
		inc		%dl
		mov		%dl, absolute_sector
		xor		%dx, %dx
		divw	heads_per_cylinder
		movb	%dl, absolute_head
		movb	%al, absolute_track
		
		pop		%dx
		ret

		/* Reads a series of sectors
		 * DL=>drive number
		 * CX=>Number of sectors to read
		 * AX=>Starting sector
		 * ES:BX=>Buffer to read to */
read_sectors:
		push	%es
		call	.read_disk_geometry
4:
		mov		$0x0005, %di /* five retries in case of error */
1: /* on error loop back here */
		push	%ax
		push	%bx
		push	%cx
		push	%dx
		call	.lba_to_chs
		mov		$0x02, %ah /* int 0x13 function 2 is read sectors */
		mov		$0x01, %al /* number of sectors to read */
		movb	absolute_track, %ch
		movb	absolute_sector, %cl
		movb	absolute_head, %dh

		int		$0x13
		jnc		1f /* test for read error */

		/* error occured use int 0x13 function 0 to reset disk */
		xor		%ax, %ax
		int		$0x13
		dec		%di /* decrement error count */
		pop		%dx
		pop		%cx
		pop		%bx
		pop		%ax
		jnz		1b
		int		$0x18 /* total failure */
1:
		mov     $dot_string, %si
		call    puts16
		pop		%dx
		pop		%cx
		pop		%bx
		pop		%ax
		addw	bytes_per_sector, %bx /* move the location where we store data */
		jc		2f
3:
		inc		%ax /* prepare to read next sector */
		loop	4b
		pop		%es
		mov     $newline_string, %si
		call    puts16
		ret
2:
		clc
		mov		%es, %dx
		addw	segment_inc, %dx
		mov		%dx, %es
		jmp		3b		
