assume cs:code,ss:stack
stack segment
     db 32 dup (0)
stack ends
code segment
    start:mov ax,stack
          mov ss,ax
          mov sp,32

          mov ax,0b800h
          mov es,ax
          mov di,160*18+40
          mov bx,offset s - offset se
          mov cx,80
        s:mov byte ptr es:[di],'!'
          mov byte ptr es:[di+1],42h
          add di,2
          int 7ch
       se:nop

          mov ax,4c00h
          int 21h
code ends
end start
