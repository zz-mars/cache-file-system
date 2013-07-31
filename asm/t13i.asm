assume cs:code,ss:stack
stack segment
     db 32 dup (0)
stack ends
code segment
    start:mov ax,stack
          mov ss,ax
          mov sp,32

          mov ax,cs
          mov ds,ax
          mov si,offset show_s
          mov cx,offset s_end - offset show_s

          mov ax,0
          mov es,ax
          mov di,200h
          cld
          rep movsb

          mov word ptr es:[7ch*4],200h
          mov word ptr es:[7ch*4+2],0

          mov ax,4c00h
          int 21h

   show_s:push ax
          push bx
          push dx
          push si

          mov bh,0
          mov bl,cl
          mov cx,1

      slp:mov ah,2
          int 10h

          mov al,[si]
          cmp al,0
          je ok
          mov ah,9
          int 10h

          inc si
          inc dl
          jmp short slp
       ok:pop si
          pop dx
          pop bx
          pop ax
          iret

    s_end:nop


code ends
end start
