assume cs:code,ds:data,ss:stack
data segment
      dd 345980
data ends
stack segment
      dw 128 dup (0)
stack ends
str segment
      db 32 dup (0)
str ends
code segment
   start:mov ax,str  ;存储转换后的字节ds
         mov ds,ax

         mov ax,stack  ;栈ss
         mov ss,ax
         mov sp,256

         mov ax,data   ;原始数据es
         mov es,ax

         mov si,0   ;字符串首地址si
         mov ax,es:[0] ;字 放入ax
         mov dx,es:[2]
         call dtoc
         ;调用子程序前准备

         mov si,0
         mov dl,41  ;列号
         mov dh,13  ;行号
         mov cl,42h
         ;调用子程序
         call show_str

         mov ax,4c00h
         int 21h

    dtoc:push bx
         push cx
         push di

         mov bx,0
      s1:mov cx,10
         call divdw
         add cl,30h
         push cx
         inc bx

         mov cx,dx
         jcxz ok0
         jmp short s1
     ok0:mov cx,ax
         jcxz ok1
         jmp short s1
     ok1:mov di,si
         mov cx,bx
       m:pop dx
         mov [di],dl
         inc di
         loop m
         mov dl,0
         mov [di],dl

         pop di
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
             jcxz ok2

             mov es:[di],cl
             mov es:[di].1,bl

             inc si
             add di,2
             jmp short s2

         ok2:pop di
             pop es
             pop dx
             pop bx
             ret

     divdw:push bx
           mov bx,ax
           mov ax,dx
           mov dx,0
           div cx
           push ax
           mov ax,bx
           div cx
           mov cx,dx
           pop dx

           pop bx
           ret
code ends
end start
