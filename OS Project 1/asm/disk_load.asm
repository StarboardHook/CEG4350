disk_load:
	pusha 
	push dx			; number of sectors (input parameter)

	mov ah, 0x02 	; read function 
	mov al, dh 		; number of sectors
	mov dl, 0x00 	; drive number
	mov dh, 0x01 	; head number
	mov ch, 0x00 	; cylinder number  
	mov cl, 0x10 	; sector number 

	; read data to [es:bx] 
	int 0x13
	jc error 		; carry bit is set -> error

	pop dx 
	cmp al, dh 		; read correct number of sectors
	jne error 

	popa 
	ret 

error:
	mov bx, error_msg
	call print 
	jmp $

print:
	pusha 

loop:
	mov al, [bx]
	cmp al, 0 
	je end 

	mov ah, 0x0e 
	int 0x10 	; print 

	add bx, 1	; next address
	jmp loop 

end:
	popa 
	ret 

error_msg: db "Error", 0 
