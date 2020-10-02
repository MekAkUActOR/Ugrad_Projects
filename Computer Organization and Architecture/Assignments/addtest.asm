;an x86 assembly language program: add two numbers
data_seg segment
    A DW 7777H, 1111H
    B DW 0AAAAH, 2222H
    C DW 0H, 0H
ends

code_seg segment
    assume cs:code_seg, ds:data_seg
start:

    MOV AX, data_seg
    MOV DS, AX
    MOV AX, A 
    ADD AX, B 
    MOV C, AX 
    MOV AX, A+2 
    ADC AX, B+2 
    MOV C+2, AX
code_seg ends
mov ax, 4c00h
int 21h  

ends

end start
