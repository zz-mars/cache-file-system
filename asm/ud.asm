assume cs:code,ds:data
data segment
     db 'bASic'
     db 'shIToYoU'
data ends
code segment
start:mov ax,data
      mov ds,ax

      mov cx,5
      mov bx,0
   s1:mov al,0dfh
      and [bx],al
      inc bx
      loop s1
      mov cx,8
   s2:mov al,00100000b
      or [bx],al
      inc bx
      loop s2

      mov ax,4c00h
      int 21h
code ends
end start
