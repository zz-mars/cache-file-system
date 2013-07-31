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
         mov si,offset lp
         mov cx,offset lp_end
         sub cx,si

         mov ax,0
         mov es,ax
         mov di,200h

         cld
         rep movsb

         mov word ptr es:[7ch*4],0200h
         mov word ptr es:[7ch*4+2],0

         mov ax,0b80h
         mov es,ax
         mov di,160*12

         mov bx,offset s - offset se
         mov cx,80
       s:mov byte ptr es:[di],'!'
         add di,2
         int 7ch
      se:nop

         mov ax,4c00h
         int 21h

      lp:push bp
         mov bp,sp
         dec cx
         jcxz lp_iret
         add [bp+2],bx
 lp_iret:pop bp
         iret
   lp_end:nop
code ends
end start
