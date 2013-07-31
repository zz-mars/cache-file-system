assume cs:code
code segment
         mov ax,0ffffh
         mov ds,ax
         mov bx,0
         mov ah,0
         mov dx,0
         mov cx,12h
   calcu:mov al,[bx]
         add dx,ax
         inc bx
         loop calcu
         mov ax,4c00h
         int 21h
code ends
end
