assume cs:code
code segment
  start:mov cx,3
        mov ax,0b800h
        mov es,ax
        mov di,720h
        mov bl,45h
        mov bh,42h
      s:mov ah,0
        int 16h
        mov es:[di],bx
        inc bl
        add di,2
        loop s

        mov ax,4c00h
        int 21h

code ends
end start
