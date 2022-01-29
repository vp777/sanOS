load_flat_gdt:
    cli
    pusha
    lgdt [gdt_descriptor]
    sti
    popa
    ret

gdt_table:
    dd 0            ; null descriptor
    dd 0

gdt_code_descriptor:
    dw 0FFFFh   ;limit low, FFFF = 65535 bytes, or 64k
    dw 0        ;ptr low, offset of mem segment
    db 0        ;ptr middle, offset of mem seg
    db 10011010b;access:
                ; 1bit is present? (required to be 1)
                ; 2bit ring level (0-3)
                ; 1bit is executable?
                ; 1bit direction (0 grows up, 1 grows down, e.g. ptr > limit)
    db 11001111b;flags+limit
    db 0        ;ptr high

gdt_data_descriptor:
    dw 0FFFFh   ;limit low (Same as code)
    dw 0        ;ptr low
    db 0        ;ptr middle
    db 10010010b;access
    db 11001111b;flags+limit
    db 0        ;ptr high

end_of_gdt:
gdt_descriptor:
    dw end_of_gdt - gdt_table - 1    ; limit (Size of GDT)
    dd gdt_table                     ; base of GDT

code_descriptor_offset equ gdt_code_descriptor - gdt_table
data_descriptor_offset equ gdt_data_descriptor - gdt_table

switch_pm32:
    call load_flat_gdt
    cli
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp code_descriptor_offset:init_pm32


