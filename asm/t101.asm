assume cs:code,ds:data,ss:stack
data segment
      db 'Welcome to masm!',0
data ends
stack segment
      db 16 dup (0)
stack ends
code segment
       start:mov ax,data
             mov ds,ax

             mov ax,stack
             mov ss,ax
             mov sp,16

             mov dh,13
             mov dl,41
             mov cl,42h
             mov si,0

             call show_str

             mov ax,4c00h
             int 21h
    show_str:push bx
             push di
             push es
             mov bx,cx
             mov ax,0b800h
             mov es,ax
             mov di,0
             mov ch,0

             mov al,160
             mul dh
             mov dh,0
             add ax,dx
             and ax,0fffeh   ;保证起始地址从偶数字节开始
             mov di,ax

           s:mov cl,[si]
             jcxz ok

             mov es:[di],cl
             mov es:[di].1,bl

             inc si
             add di,2
             jmp short s

          ok:pop es
             pop di
             pop bx
             ret
code ends
end start
