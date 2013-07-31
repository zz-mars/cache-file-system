assume cs:code,ds:data,ss:stack
data segment
     dw 256 dup (0)
data ends
stack segment
     dw 32 dup (0)
stack ends
code segment
    start:mov ax,stack
          mov ss,ax
          mov sp,64

          mov ax,data
          mov ds,ax
          mov si,0  ;存储字符的起始地址

          mov dh,8  ;初始行
          mov dl,20  ;初始列

     gets:mov ah,0
          int 16h
          cmp al,20h
          jb notchar

          mov ah,0
          call ed_str

          mov ah,2
          call ed_str
          jmp short gets

  notchar:cmp ah,0eh
          je backspace
          cmp ah,1ch
          je enter
          jmp short gets

backspace:mov ah,1
          call ed_str

          mov ah,2
          call ed_str
          jmp short gets

    enter:mov ax,0   ;0入栈
          call ed_str

          mov ah,2
          call ed_str

          mov ax,4c00h
          int 21h

   ed_str:jmp short s
       table dw pushc,popc,shows
       top   dw 0         ;指示栈顶，初始为0，栈顶的值永远都是0
        s:push bx
          mov bl,ah
          mov bh,0
          add bx,bx
          call table[bx]
          pop bx
          ret

    pushc:push di
          mov di,top
          mov ah,0
          mov [di],ax
          inc di
          mov top,di
          pop di
          ret

     popc:push di
          mov di,top
          cmp di,0
          je popok
          dec di
          and byte ptr [di],0
          mov top,di
    popok:pop di
          ret

    shows:push ax
          push dx
          push es
          push si
          push di

          mov ax,0b800h
          mov es,ax
          mov al,160
          mul dh
          mov dh,0
          add dx,dx
          add ax,dx
          mov di,ax

        n:mov al,[si]
          cmp al,0
          je show_ok
          mov ah,42h
          mov es:[di],ax
          inc si
          add di,2
          jmp short n

  show_ok:pop di
          pop si
          pop es
          pop dx
          pop ax
          ret
code ends
end start
