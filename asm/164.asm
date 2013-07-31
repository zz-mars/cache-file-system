assume cs:code,ss:stack
stack segment
      dw 64 dup (0)
stack ends
code segment
   start:mov ax,stack
         mov ss,ax
         mov sp,128

         call sub4

         mov ax,4c00h
         int 21h

    sub1:push bx
         push cx
         push es

         mov bx,0b800h
         mov es,bx
         mov di,0
         mov cx,2000
      s1:mov byte ptr es:[bx],' '
         add bx,2
         loop s1

         pop es
         pop cx
         pop bx

         ret

    sub4:push bx
         push cx
         push es
         push si

         mov bx,0b800h
         mov es,bx

         mov cx,24
         mov bx,0
     s44:push cx
         mov cx,80
         mov si,0
      s4:push es:[bx].160[si]
         pop es:[bx][si]
         add si,2
         loop s4
         pop cx
         add bx,160
         loop s44

         pop si
         pop es
         pop cx
         pop bx
         ret

code ends
end start
