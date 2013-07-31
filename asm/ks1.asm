assume cs:code,ds:data,ss:stack

data segment

    db '1975','1976','1977','1978','1979','1980','1981','1982','1983'

    db '1984','1985','1986','1987','1988','1989','1990','1991','1992'

    db '1993','1994','1995'

    dd 16,22,382,1356,2390,8000,16000,24486,50065,97479,140417,197514

    dd 345980,590827,803530,1183000,1843000,2759000,3753000,4649000,5937000

    dw 3,7,9,13,28,38,130,220,476,778,1001,1442,2258,2793,4037,5635,8226

    dw 11452,14430,15257,17800

data ends
byte_str segment
    db 32 dup (0)
byte_str ends
stack segment
    dw 64 dup (0)
stack ends
code segment
   start:mov ax,data
         mov es,ax

         mov ax,byte_str
         mov ds,ax

         mov ax,stack
         mov ss,ax
         mov sp,128

         mov cx,21
         mov bx,0
         mov si,0  ;指示年份和收入，都是4字节，采用相同方式定位
         mov di,0  ;指示员工数

         mov dh,2  ;初始化行号
         mov dl,0
         push dx

       s:mov ax,es:[bx][si]
         mov ds:[0],ax
         mov ax,es:[bx][si+2]
         mov ds:[2],ax
         mov byte ptr ds:[4],0

         pop dx     ;获得行号，开始为调用show_str显示年份做准备
         push cx  ;保存cx值
         push si  ;保存si值

         mov dl,20   ;初始化年份的列号
         mov cl,42h
         mov si,0
         call show_str

         pop si
         push dx
         mov dx,es:[bx].84[si+2]
         mov ax,es:[bx].84[si]
         push si ;重新保护
         mov si,0
         call dtoc
         pop si
         pop dx
         push si
         mov dl,34
         mov cl,71h
         mov si,0
         call show_str

         push dx    ;主程序中保护dx，使dtoc子程序具有更大的使用范围和灵活性
         mov dx,0
         mov ax,es:[bx].168[di]   ;寻址员工数
         mov si,0
         call dtoc
         pop dx
         mov dl,70
         mov cl,0c2h
         mov si,0
         call show_str

         pop si ;寻址需要，拿出si
         push dx  ;需要dx，保护原值
         mov dx,es:[bx].84[si+2]
         mov ax,es:[bx].84[si]
         mov cx,es:[bx].168[di]
         call divdw
         push si
         mov si,0
         call dtoc
         pop si
         pop dx
         push si
         mov dl,88
         mov cl,4ah
         mov si,0
         call show_str

         pop si
         pop cx

         add si,4
         add di,2
         add dh,2   ;更新行
         push dx
         dec cx
         jcxz over
         jmp near ptr s
    over:mov ax,4c00h
         int 21h

    dtoc:push bx
         push cx
    ;     push dx        ;把dx的保护放在主程序中
         push di

         mov bx,0
     ;    mov dx,0     ;在主程序中为dx赋值
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
        ; mov bp,cx  ;用bp保存每个16位二进制数转成的十进制数的位数
       m:pop dx
         mov [di],dl
         inc di
         loop m
         mov dl,0
         mov [di],dl

         pop di
      ;   pop dx
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

     divdw:push bx   ;防溢出除法，被除数高16位dx，低16位ax，商存在dx-ax中，余数在cx中
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

