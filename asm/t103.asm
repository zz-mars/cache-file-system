assume cs:code,ds:data,ss:stack
data segment
      dw 123,126,1,8,3,38
data ends
stack segment
      dw 64 dup (0)
stack ends
str segment
      db 32 dup (0)
str ends
code segment
   start:mov ax,str
         mov ds,ax

         mov ax,stack
         mov ss,ax
         mov sp,128

         mov ax,data
         mov es,ax
         mov bp,0
         mov cx,6
         mov si,0   ;字符串首地址

         mov dl,41  ;列号
         mov dh,13  ;行号
         push dx
    ;     mov cl,42h  ;颜色
    ;     call show_str
    ;     inc dl

      s0:mov ax,es:[bp] ;字 放入ax
         call dtoc
         pop dx
         push cx
         push si

         mov si,0
         mov cl,42h

         call show_str
         inc dh
         pop si
         pop cx
         push dx
         add bp,2
         loop s0

         mov ax,4c00h
         int 21h

    dtoc:push bx
         push cx
         push dx

         mov ch,0
         mov dh,10
         mov dl,0
      s1:div dh
         add ah,30h
         push ax
         inc dl
         mov cl,al
         jcxz ok0
         mov ah,0
         jmp short s1

     ok0:mov di,si
         mov cl,dl
         mov ch,0
       m:pop dx
         mov [di],dh
         inc di
         loop m
         mov dh,0
         mov [di],dh


         pop dx
         pop cx
         pop bx
         ret

    show_str:push bx
             push dx
             push es
             push di
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

          s2:mov cl,[si]
             jcxz ok1

             mov es:[di],cl
             mov es:[di].1,bl

             inc si
             add di,2
             jmp short s2

         ok1:pop di
             pop es
             pop dx
             pop bx
             ret
code ends
end start
