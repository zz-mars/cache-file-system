assume cs:code,ss:stack
stack segment
    dw 8 dup (0)
stack ends
code segment
     start:mov ax,stack
           mov ss,ax
           mov sp,16

           mov dx,0
           mov ax,12666
           mov cx,10
           call divdw

           mov ax,4c00h
           int 21h

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
