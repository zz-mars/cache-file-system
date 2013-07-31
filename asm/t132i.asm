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
          mov si,offset lp
          mov cx,offset lp_end - offset lp

          mov ax,0
          mov es,ax
          mov di,200h
          cld
          rep movsb

          mov word ptr es:[7ch*4],200h
          mov word ptr es:[7ch*4+2],0

          mov ax,4c00h
          int 21h

       lp:push bp
          mov bp,sp
          dec cx
          jcxz ok
          add [bp+2],bx
       ok:pop bp
          iret
      lp_end:nop


code ends
end start
