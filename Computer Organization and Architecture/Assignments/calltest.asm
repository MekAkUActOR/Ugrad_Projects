;an x86 assembly language program: call a function
data_seg segment
   C DW 0H
ends

stack_seg segment
    ST DB 100DUP(0)
    TOP EQU 100
ends

code_seg segment 
    assume cs:code_seg, ds:data_seg, ss:stack_seg
start:

    MOV AX, data_seg 
    MOV DS, AX
    MOV AX, stack_seg 
    MOV SS, AX
    MOV SP, TOP
    MOV BX, 5 
    PUSH 9
    PUSH 7
    CALL compute 
    ADD BX, AX 
    MOV C, BX

mov ax, 4c00h
int 21h  
   
    compute proc near 
        PUSH BX
        PUSH DX
        PUSH SI
        MOV SI, SP
        MOV AX, SS:[SI + 10] 
        MOV BX, 10
        MUL BX
        ADD AX, SS:[SI + 8] 
        POP SI
        POP DX
        POP BX
        RET
compute endp
   
ends

end start
