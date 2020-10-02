;an x86 assembly language program: Quick Sort 50 numbers
data_seg1 segment
    Rand DW -1478,-1435,9717,12670,24787,-9063,-10763,-6781,15135,-30123,-1507,-29927,7631,6202,-18157,-19756,-18989,16501,-8657,1,20283,-15774,-14328,-15627,32579,8083,8224,1,-8137,26670,-22478,-30766,1,-16845,16598,-7502,18556,-4855,3713,26864,-30386,27752,11721,7193,1,21640,-5703,-20051,5387,-28702

    x DW  0h ;100 101
    i DW  0h ;102 103
    j DW  0h ;104 105   
ends

stack_seg segment
    ST   DB   1000  dup(0)
    TOP EQU 1000
ends

code_seg segment 
    assume cs:code_seg, ds:data_seg1, ss:stack_seg
start:

    MOV AX, data_seg1 
    MOV DS, AX
    MOV AX, stack_seg 
    MOV SS, AX
    MOV SP, TOP
    MOV BX, 0
    MOV CX, 98
    PUSH BX
    PUSH CX
    CALL QUICKSORT    

mov ax, 4c00h
int 21h

    QUICKSORT proc near
        PUSH SI
        PUSH AX
        PUSH BX
        PUSH CX
        MOV SI, SP
        MOV BX, SS:[SI + 12]
        MOV CX, SS:[SI + 10]        
        SUB BX, CX
        JNS pad1
        ADD BX, CX
        PUSH BX
        PUSH CX
        CALL PARTITION
        POP CX
        POP BX
        MOV AX, i
        SUB AX, 2;q-2
        PUSH BX
        PUSH AX
        CALL QUICKSORT
        POP AX
        POP BX
        ADD AX, 2
        PUSH AX;q
        PUSH CX
        CALL QUICKSORT
        POP CX 
        POP AX         
        ;pad1
pad1:   POP CX
        POP BX
        POP AX
        POP SI      
    RET    
    QUICKSORT endp
    
    PARTITION proc near
        PUSH AX
        PUSH DX
        PUSH SI
        MOV SI, SP
        MOV DX, SS:[SI + 8];DX=r
        MOV BX, DX
        MOV AX, [BX];AX=x=Q[r]
        MOV x, AX
        MOV BX, SS:[SI + 10]
        MOV i, BX;i=p, BX=j=p
        SUB i, 2;i=p-2
        ;pad3
pad3:   MOV CX, [BX];CX=Q[j]
        MOV AX, x
        SUB AX, CX;x>=Q[j]|watch overflow|
        JNS pad21
        JS pad41
        
pad22:  ADD i, 2;i++++
        MOV j, BX
        MOV BX, i
        MOV AX, [BX];DX=Q[i]
        MOV BX, j;BX=j   **********
        MOV [BX], AX;Q[j]=DX=Q[i]
        MOV BX, i
        MOV [BX], CX;Q[i]=CX=Q[j]
        MOV BX, j
        JMP pad5
        ;pad2
pad21:  JNO pad22
        JMP pad5
pad41:  JO pad22
pad5:   ADD BX, 2;j++++
        MOV AX, BX ;AX=j
        MOV CX, DX ;CX=r
        SUB AX, CX
        JS pad3
        ADD i, 2;i+2
        MOV j, BX
        MOV BX, i
        MOV AX, [BX];AX=Q[i]
        MOV BX, DX
        MOV [BX], AX;Q[r]=AX=Q[i]
        MOV BX, i
        MOV AX, x;AX=Q[r]
        MOV [BX], AX;Q[i]=AX=Q[r]           
        POP SI
        POP DX
        POP AX;return is in i(mem16)
    RET
    PARTITION endp

ends

end start
