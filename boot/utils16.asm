kernel_load:

push dx ;the number of requested sectors

mov ah, 0x02 ;bios read sector function
mov al, dh   ;read dh sectors
mov ch, 0x00 ;select cylinder 0
mov cl, 0x02 ;start reading from the second sector
mov dh, 0x00 ;select head 0
int 0x13     ;bios interrupt

;jc disk_error ;jump if error/carry flag is set

pop dx
cmp dh, al
jne disk_error

ret

disk_error:

    mov bx, DISK_ERROR_MESSAGE
    call prints
    jmp $
    
DISK_ERROR_MESSAGE db "Error reading the kernel", 0

;;;;;;;;;;;;;;;;;;;;

printc:
pusha
mov al, dl
mov ah, 0x0e
int 0x10
popa
ret

prints:

pusha
mov ah, 0x0E

prints_loop:
    mov al, [bx]
    cmp al, 0
    je prints_end
    
    int 0x10
    inc bx
    jmp prints_loop

prints_end:
popa
ret


hex_alphabet: db '0123456789abcdef'

print_hex:

pusha
mov ah, 0x0E
mov si, hex_alphabet
mov di, bx

print_hex_loop:
    cmp cx, 0
    je print_hex_end
    movzx bx, [di]
    mov dx, bx
    
    
    shr bl, 4
    mov al, [si+bx]
    int 0x10
    
    mov bx, dx
    and bl, 0xf
    mov al, [si+bx]
    int 0x10
    
    dec cx
    inc di
    jmp print_hex_loop

print_hex_end:
popa
ret

;bios e820 to detect memory usage, from osdev
mmap_ent equ 0x8800             ; the number of entries will be stored at 0x8000
do_e820:
    lea di, [mmap_ent+4]          ; Set di to 0x8004. Otherwise this code will get stuck in `int 0x15` after some entries are fetched
	xor ebx, ebx		; ebx must be 0 to start
	xor bp, bp		; keep an entry count in bp
	mov edx, 0x0534D4150	; Place "SMAP" into edx
	mov eax, 0xe820
	mov [es:di + 20], dword 1	; force a valid ACPI 3.X entry
	mov ecx, 24		; ask for 24 bytes
	int 0x15
	jc short .failed	; carry set on first call means "unsupported function"
	mov edx, 0x0534D4150	; Some BIOSes apparently trash this register?
	cmp eax, edx		; on success, eax must have been reset to "SMAP"
	jne short .failed
	test ebx, ebx		; ebx = 0 implies list is only 1 entry long (worthless)
	je short .failed
	jmp short .jmpin
.e820lp:
	mov eax, 0xe820		; eax, ecx get trashed on every int 0x15 call
	mov [es:di + 20], dword 1	; force a valid ACPI 3.X entry
	mov ecx, 24		; ask for 24 bytes again
	int 0x15
	jc short .e820f		; carry set means "end of list already reached"
	mov edx, 0x0534D4150	; repair potentially trashed register
.jmpin:
	jcxz .skipent		; skip any 0 length entries
	cmp cl, 20		; got a 24 byte ACPI 3.X response?
	jbe short .notext
	test byte [es:di + 20], 1	; if so: is the "ignore this data" bit clear?
	je short .skipent
.notext:
	mov ecx, [es:di + 8]	; get lower uint32_t of memory region length
	or ecx, [es:di + 12]	; "or" it with upper uint32_t to test for zero
	jz .skipent		; if length uint64_t is 0, skip entry
	inc bp			; got a good entry: ++count, move to next storage spot
	add di, 24
.skipent:
	test ebx, ebx		; if ebx resets to 0, list is complete
	jne short .e820lp
.e820f:
	mov [mmap_ent], bp	; store the entry count
	clc			; there is "jc" on end of list to this point, so the carry must be cleared
	ret
.failed:
	stc			; "function unsupported" error exit
	ret
