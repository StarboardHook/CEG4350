[bits 16]

switch:
	cli
	lgdt [gdt_descriptor]
	
	mov eax, cr0 
	or eax, 0x1 
	mov cr0, eax		; Switch to protected mode

	; Enable SSE instructions
	mov eax, cr0
	and ax, 0xFFFB		;clear coprocessor emulation CR0.EM
	or ax, 0x2			;set coprocessor monitoring  CR0.MP
	mov cr0, eax
	mov eax, cr4
	or ax, 3 << 9		;set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
	mov cr4, eax

	jmp CODE_SEG:init 	; Flush pipeline  

[bits 32]
init:
	mov ax, DATA_SEG 
	mov ds, ax 
	mov ss, ax 
	mov es, ax 
	mov fs, ax 
	mov gs, ax 

	mov ebp, 0x80000
	mov esp, ebp 

	call pmode 
