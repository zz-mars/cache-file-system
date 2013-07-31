assume cs:code,ds:data,ss:stack
data segment
       db 'word',0
       db 'unix',0
       db 'wind',0
       db 'good',0
data ends
stack segment
       dw 8 dup (0)
stack ends
code segment
    start:mov ax,data
          mov ds,ax
          mov ax,stack
          mov ss,ax
          mov sp,16

          mov bx,0
          mov cx,4
          mov al,11011111b
        s:call capital
          add bx,5
          loop s

          mov ax,4c00h
          int 21h

  capital:push cx
          mov si,0
          mov ch,0
        l:mov cl,[bx][si]
          jcxz ok
          and [bx][si],al
          inc si
          jmp short l
       ok:pop cx
          ret
code ends
end start
