assume cs:code,ds:data,ss:stack
data segment
     db 512 dup (0)
data ends
stack segment
     dw 16 dup (0)
stack ends
code segment
    start:mov ax,data
          mov ds,ax

          mov ax,stack
          mov ss,ax
          mov sp,32

          mov dh,8  ;初始行
          mov dl,20  ;初始列

          mov si,0

        s:mov ah,0
          int 16h
          cmp al,20h  ;是否为字符
          jb notchar
          mov bl,42h
          call set_cur
          call show_char
          mov [si],al
          inc si
          inc dl
          cmp dl,80
          jne s
          inc dh
          mov dl,20
          jmp short s
  notchar:cmp ah,1ch  ;回车
          je enter
          cmp ah,0eh
          je backspace
          jmp short s
backspace:cmp si,0
          je s
          mov al,0
          mov bl,07h
          dec si
          dec dl
          cmp dl,20
          jnb t
          dec dh
          mov dl,79
        t:call set_cur
          call show_char
          jmp short s

    enter:mov byte ptr [si],0

          mov si,0
          mov di,0
          mov ax,0b800h
          mov es,ax
          mov bx,720h
        n:mov al,[si]
          cmp al,0
          je over
          mov ah,4ah
          mov es:[bx][di],ax
          add di,2
          inc si
          jmp short n


     over:mov ax,4c00h
          int 21h

  set_cur:push ax   ;设置光标
          push bx
          mov bh,0
          mov ah,2
          int 10h
          pop bx
          pop ax
          ret

show_char:push ax   ;光标处显示字符
          push bx
          push cx

          mov bh,0
          mov cx,1
          mov ah,9
          int 10h

          pop cx
          pop bx
          pop ax
          ret
code ends
end start
