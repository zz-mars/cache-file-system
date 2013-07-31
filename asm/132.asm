assume cs:code,ss:stack
stack segment
     dw 64 dup (0)
stack ends
code segment
   start:mov ax,stack
         mov ss,ax
         mov sp,128
         mov ax,cs
         mov ds,ax
         mov si,offset squr
         mov cx,offset s_end
         sub cx,si

         mov ax,0
         mov es,ax
         mov di,200h

         cld
         rep movsb

         mov word ptr es:[496],0200h
         mov word ptr es:[498],0

         mov ax,3456
         pushf
         pop bx
         int 7ch

         mov ax,4c00h
         int 21h

    squr:pushf
         pop bx
         mul ax
         iret
   s_end:nop
code ends
end start
