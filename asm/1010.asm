assume cs:code,ds:data,ss:stack
data segment
        dw 337,2,3,4,5,6,7,8
        dd 8 dup (0)
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

          mov cx,8
          mov si,0
          mov di,16

        s:mov bx,[si]
          call cube1
          mov [di],ax
          mov [di].2,dx
          add si,2
          add di,4
          loop s

          mov ax,4c00h
          int 21h

     cube:push cx       ;还是有缺陷，用第一次乘法后dx和ax分别与bx相乘，要保证dx与bx乘积不超过16位
          mov ax,bx     ;否则，bx的三次方超过32位数能表示的范围
          mul bx
          push ax
          mov ax,dx
          mul bx
          mov dx,ax
          pop ax
          push dx
          mul bx
          pop cx
          add dx,cx
          pop cx
          ret
    cube1:push cx
          mov ax,bx
          mul bx
          mov cx,ax
          mov ax,dx
          mul bx
          push ax
          mov ax,cx
          mul bx
          pop cx
          add dx,cx
          pop cx
          ret
code ends
end start
