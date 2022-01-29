[bits 32]
[extern kernel_main]
[extern memory_map_count]

mov al, 'K'
mov [0xb8000+12],al
mov eax, [esp+4]
mov eax, [eax]
mov byte [memory_map_count], al
call kernel_main

