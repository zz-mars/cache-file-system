assume cs:code,ds:data
data segment
     db 9,8,7,4,2,0
     db '// ::',0
data ends
code segment
     start:mov ax,data
           mov ds,ax

           mov ax,0b800h
           mov es,ax
           mov di,720h

           mov si,0
           mov cx,6
           mov dh,42h
         s:mov al,[si]
           out 70h,al
           in al,71h
           call bcd2d2
           mov dl,ah
           mov es:[di],dx
           mov dl,al
           mov es:[di+2],dx
           mov dl,[si+6]
           mov es:[di+4],dx
           inc si
           add di,6
           loop s

           mov ax,4c00h
           int 21h

     bcd2d:push bx
           push cx

           mov bl,10
           mov bh,al
           and ax,000fh
           div bl
           add ah,30h
           mov al,bh
           mov bh,ah
           mov cl,4
           shr al,cl
           and ax,000fh
           div bl
           add ah,30h
           mov al,bh

           pop cx
           pop bx
           ret
    bcd2d2:push cx
           mov cl,4
           shl ax,cl
           shr al,cl

           and ax,0f0fh
           add ax,3030h
           pop cx
           ret
code ends
end start
