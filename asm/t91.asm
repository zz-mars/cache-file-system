assume cs:code,ds:data
data segment
       db 'welcome to masm!'
       db 42h,0c2h,0f1h
data ends
code segment
   start:mov ax,data
         mov ds,ax

         mov ax,0b800h
         mov es,ax

         mov cx,16
         mov bx,0
         mov di,0

       s:mov al,[bx]
         mov ah,ds:[16]
         mov es:[di].720h,ax
         mov ah,ds:[17]
         mov es:[di].720h.160,ax
         mov ah,ds:[18]
         mov es:[di].720h.320,ax
         inc bx
         add di,2
         loop s

         mov ax,4c00h
         int 21h
code ends
end start
