;an x86 assembly language program: Modify keyboard interrupt: Move all keys backward one space in sequence
data segment
    count dw ?
DATA ENDS
code segment
    assume cs:code,ds:data
    jmp start
newint9 proc
    sti
    pushf
    push ax
    push bx
    in al,60h;get key code
    push ax
    in al,61h;get current control
    mov ah,al
    or al,80h;set control 1xxx xxxx clear kbd info
    out 61h,al
    xchg al,ah
    out 61h,al

    pop ax  ;keyboard code in al
    cmp al, 0bh
    je re0
    cmp al, 02h
    je re1
    cmp al, 03h
    je re2
    cmp al, 04h
    je re3
    cmp al, 05h
    je re4
    cmp al, 06h
    je re5
    cmp al, 07h
    je re6
    cmp al, 08h
    je re7
    cmp al, 09h
    je re8
    cmp al, 0ah
    je re9
    cmp al, 0ch
    je res
    cmp al, 0dh
    je ree
    cmp al, 1ah
    je rel
    cmp al, 1bh
    je rer
    cmp al, 27h
    je ref
    cmp al, 28h
    je redy
    cmp al, 2bh
    je relx
    cmp al, 33h
    je redo
    cmp al, 34h
    je reju
    cmp al, 35h
    je rerx
    jmp char
re0:jmp rel0
re1:jmp rel1
re2:jmp rel2
re3:jmp rel3
re4:jmp rel4
re5:jmp rel5
re6:jmp rel6
re7:jmp rel7
re8:jmp rel8
re9:jmp rel9
res:jmp ress
ree:jmp rese
rel:jmp resl
rer:jmp resr
ref:jmp resf
redy:jmp resdy
relx:jmp reslx
redo:jmp resdo
reju:jmp resju
rerx:jmp resrx
char: 
    cmp al,10h
    je  q1
    cmp al,11h
    je  w1
    cmp al,12h
    je  e1
    cmp al,13h
    je  r1
    cmp al,14h
    je  t1
    cmp al,15h
    je  y1
    cmp al,16h
    je  u1
    cmp al,17h
    je i1
    cmp al,18h
    je o1
    cmp al,19h
    je p1
    cmp al,1eh
    je a1
    cmp al,1fh
    je s1
    cmp al,20h
    je d1
    cmp al,21h
    je f1
    cmp al,22h
    je g1
    cmp al,23h
    je h1
    cmp al,24h
    je j1
    cmp al,25h
    je k1
    cmp al,26h
    je l1
    cmp al,2ch
    je z1
    cmp al,2dh
    je x1
    cmp al,2eh
    je c1
    cmp al,2fh
    je v1
    cmp al,30h
    je b1
    cmp al,31h
    je n1
    cmp al,32h
    je m1
    jmp nxt1
d1: jmp d
g1: jmp g
o1: jmp o
p1: jmp p
s1: jmp s
z1: jmp z
x1: jmp x
v1: jmp v
q1: jmp q
w1: jmp w
e1: jmp e
r1: jmp r
t1: jmp t
y1: jmp y
u1: jmp u
i1: jmp i
j1: jmp j
k1: jmp k
l1: jmp l
a1: jmp a
f1: jmp f
h1: jmp h
c1: jmp c
b1: jmp b
n1: jmp n
m1: jmp m
rel0:
    mov ax, 0e30h
    jmp nxt
rel1:
    mov ax, 0e31h
    jmp nxt
rel2:
    mov ax, 0e32h
    jmp nxt
rel3:
    mov ax, 0e33h
    jmp nxt
rel4:
    mov ax, 0e34h
    jmp nxt
rel5:
    mov ax, 0e35h
    jmp nxt
rel6:
    mov ax, 0e36h
    jmp nxt
rel7:
    mov ax, 0e37h
    jmp nxt
rel8:
    mov ax, 0e38h
    jmp nxt
rel9:
    mov ax, 0e39h
    jmp nxt
ress:
    mov ax, 0e2dh
    jmp nxt
rese:
    mov ax, 0e3dh
    jmp nxt
resl:
    mov ax, 0e5bh
    jmp nxt
resr:
    mov ax, 0e5dh
    jmp nxt
resf:
    mov ax, 0e3bh
    jmp nxt
resdy:
    mov ax, 0e27h
    jmp nxt
reslx:
    mov ax, 0e5ch
    jmp nxt
resdo:
    mov ax, 0e2ch
    jmp nxt
resju:
    mov ax, 0e2eh
    jmp nxt
resrx:
    mov ax, 0e2fh
    jmp nxt  
a:  mov ax,0e62h
    jmp nxt
b:  mov ax,0e63h
    jmp nxt
c:  mov ax,0e64h
    jmp nxt
d:  mov ax,0e65h
    jmp nxt
e:  mov ax,0e66h
    jmp nxt
f:  mov ax,0e67h
    jmp nxt
g:  mov ax,0e68h
    jmp nxt
h:  mov ax,0e69h
    jmp nxt
i:  mov ax,0e6ah
    jmp nxt
j:  mov ax,0e6bh
    jmp nxt
k:  mov ax,0e6ch
    jmp nxt
l:  mov ax,0e6dh
    jmp nxt
m:  mov ax,0e6eh
    jmp nxt
n:  mov ax,0e6fh
    jmp nxt
o:  mov ax,0e70h
    jmp nxt
p:  mov ax,0e71h
    jmp nxt
q:  mov ax,0e72h
    jmp nxt
r:  mov ax,0e73h 
    jmp nxt
s:  mov ax,0e74h
    jmp nxt
t:  mov ax,0e75h
    jmp nxt
u:  mov ax,0e76h
    jmp nxt
v:  mov ax,0e77h
    jmp nxt
w:  mov ax,0e78h
    jmp nxt
x:  mov ax,0e79h
    jmp nxt
y:  mov ax,0e7ah
    jmp nxt
z:  mov ax,0e61h
nxt:int 10h;print secretcode
nxt1:
    mov al,20h
    out 20h,al ;oridinary EOI
    sti
    pop bx
    pop ax
    popf
    iret
newint9 endp


newint1c proc
    sti
    pushf
    push ax
    add count,1
    cmp count,182
    jb ext
    mov count,0
    
    in al,61h
    xor al,0c0h
    out 61h,al
    xor al,0c0h
    out 61h,al

    in al,21h
    xor al,02h
    out 21h,al
ext:
    pop ax
    popf  
    iret
newint1c endp

start:
    push ds  
    push ax
    mov ax,data
    mov ds,ax
    mov count,182
    cli
    ;save int9
    mov ax,2509h
    mov dx,seg newint9
    mov ds,dx
    mov dx,offset newint9
    int 21h   ;change the int9 entrence
    
    mov ax,251ch
    mov dx,seg newint1c
    mov ds,dx
    mov dx,offset newint1c
    int 21h   ;change the 
    pop ax
    pop ds
    mov dx,offset start + 15
    int 27h
code ends
end
