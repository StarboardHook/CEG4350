[org 0x7C00]

kernel_offset equ 0x1000

jmp short _start
nop

; FAT12 Bios Parameter Block
oem						db "MSWIN4.1"
bytesPerSector			dw 512
sectorsPerCluster		db 1
reservedSectors			dw 1
fatCount				db 2
rootDirectoryEntries	dw 224
sectorCount				dw 2880
mediaDescriptorType		db 0b11111000
sectorsPerFat			dw 9
sectorsPerTrack			dw 18
headCount				dw 2
hiddenSectorCount		db 0
largeSectorCount		dd 0

; Extended Boot Record
driveNumber				db 0
reserved				db 0
signature				db 29h
volumeID				db 00h, 00h, 00h, 00h
volumeLabel				db "BOOT FLOPPY"
systemID				db "FAT16   "

_start:
	mov bp, 0x8000		; Setup stack and frame pointers
	mov sp, bp
	call load_kernel	; Load the kernel
	call switch			; Switch to protected mode
	jmp $

%include "./asm/disk_load.asm"
%include "./asm/gdt.asm"
%include "./asm/switch.asm"

[bits 16]
load_kernel:
	mov bx, kernel_offset 
	mov dh, 48			; Load 18 (the maximum) amount of sectors in the first head and cylinder
	call disk_load		; Load the disk so we can properly start the kernel

	; Put your code here to disable the blinking cursor
	; The blinking cursor can only be disabled in real mode using BIOS interrupt int 0x10
	mov ah, 0x01	; Function Mode: Change Cursor Shape
	mov cx, 0x2607	; Black cursor CX=2607
	int 0x10		; BIOS interrupt
	
	ret

[bits 32]
pmode:
	call kernel_offset
	jmp $

times 510 - ($ - $$) db 0
db 0x55, 0xaa
