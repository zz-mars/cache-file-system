assume cs:code
stack segment
     db 128 dup (0)
stack ends
data segment
     dw 0,0
data ends
code segment
    start:mov ax,stack
          mov ss,ax
          mov sp,128

          mov ax,data
          mov ds,ax

          mov ax,0
          mov es,ax

          mov ax,es:[9*4]
          mov ds:[0],ax
          mov ax,es:[9*4+2]
          mov ds:[2],ax

          mov ax,offset int9
          mov word ptr es:[9*4],ax
          mov ax,cs
          mov word ptr es:[9*4+2],ax

          int 9h

          mov ax,0
          mov es,ax

          mov ax,[0]
          mov es:[9*4],ax
          mov ax,[2]
          mov es:[9*4+2],ax

          mov ax,4c00h
          int 21h

     int9:push ax
          push bx
          push es

          pushf
          call dword ptr ds:[0]

   int9rt:pop es
          pop bx
          pop ax
          iret
code ends
end start
