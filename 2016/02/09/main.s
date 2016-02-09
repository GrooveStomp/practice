BITS 64

section .data
msg db 'The sum is: ', 0xa      ; 0xa is linefeed, aka newline
len equ $ - msg
lf  db 0xa

section .bss
        res resb 1              ; `res` is a variable 1 'reserved byte' long

SECTION .text

adder:
        mov eax, ecx
        add eax, edx
        ret

        global _start

_start:
        mov ecx, 4
        mov edx, 5
        call adder

        add eax, '0'            ; add ascii zero to convert to decimal character
        mov [res], eax          ; move ascii decimal back to variable

        mov edx, len            ; message length parameter
        mov ecx, msg            ; messsage string parameter
        mov ebx, 1              ; stdout parameter
        mov eax, 4              ; system call (sys_write)
        int 0x80                ; call kernel

        mov edx, 1              ; message length parameter
        mov ecx, res            ; move adder result to register
        mov ebx, 1              ; stdout parameter
        mov eax, 4              ; system call (sys_write)
        int 0x80                ; call kernel

        mov edx, 1              ; message length parameter
        mov ecx, lf             ; move adder result to register
        mov ebx, 1              ; stdout parameter
        mov eax, 4              ; system call (sys_write)
        int 0x80                ; call kernel

        mov eax, 1              ; system call (sys_exit)
        int 0x80                ; call kernel
