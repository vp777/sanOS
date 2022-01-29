[bits 16]

[org 0x7C00]

global memory_map_count

KERNEL_OFFSET equ 0x9000
STACK_END equ 0x00EFFFFF

main:
mov [BOOT_DRIVE], dl

mov sp, 0x8000

mov bx, msg
mov cx, 4
call prints

mov bx, KERNEL_OFFSET
mov dh, 30 ;number of sectors to read
mov dl, [BOOT_DRIVE]
call kernel_load


call do_e820
mov [memory_map_count], bp
call switch_pm32

%include "utils16.asm"
%include "pm32.asm"

[bits 32]
init_pm32:
    mov ax, data_descriptor_offset
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov ebp, STACK_END
    mov esp, ebp
    push memory_map_count
    call KERNEL_OFFSET


msg: db 'sanOS',0
BOOT_DRIVE: db 0
memory_map_count: db 0
times 510-($-$$) db 0
dw 0xaa55

