assume cs:code
code segment
    start:mov ax,0
          mov es,ax
          mov di,200h

          mov ax,cs            ;安装前先保护原来的中断处理程序入口到现在的中断程序中
          mov ds,ax
          mov si,offset int9
          mov ax,es:[9*4]
          mov [si+2],ax
          mov ax,es:[9*4+2]
          mov [si+4],ax

          mov cx,offset int9_end - offset int9
          cld
          rep movsb

          cli
          mov ax,200h
          mov es:[9*4],ax
          mov ax,0
          mov es:[9*4+2],ax
          sti

          mov ax,4c00h
          int 21h

     int9:jmp short s
          dw 0,0
        s:push ax
          push cx
          push es
          push si

          mov ax,cs
          mov ds,ax
          mov si,202h

          in al,60h

          pushf
          call dword ptr [si]



          cmp al,3bh
          jne ok
          mov cx,2000
          mov si,1
          mov ax,0b800h
          mov es,ax
       lp:inc byte ptr es:[si]
          add si,2
          loop lp

       ok:pop si
          pop es
          pop cx
          pop ax
          iret
 int9_end:nop
code ends
end start
