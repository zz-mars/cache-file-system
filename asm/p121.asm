assume cs:code,ds:data,ss:stack
data segment
    db 'back to main programs!',0
data ends
stack segment
     dw 64 dup (0)
stack ends
code segment
    start:mov ax,stack
          mov ss,ax
          mov sp,128

          mov ax,0
          mov es,ax
          mov di,0200h

          mov ax,cs
          mov ds,ax
          mov si,offset do0
          mov cx,offset over
          sub cx,si

          cld
          rep movsb    ;安装完毕

          mov dx,0
          mov ax,0200h
          mov es:[0],ax
          mov es:[2],dx  ;中断向量设置完毕

          ;mov dx,1     ;测试中断程序
          ;mov ax,0
          ;mov bx,1
          ;div bx
          int 0
          nop
          nop

          mov ax,data
          mov ds,ax
          mov si,0

          mov ax,0b800h
          mov es,ax
          mov di,14*160+32*2
       s0:mov cl,[si]
          mov ch,0
          jcxz ok0
          mov ch,42h
          mov es:[di],cx
          add di,2
          inc si
          jmp short s0

      ok0:mov ax,4c00h
          int 21h

      do0:jmp short begin
          db 'overflow!',0
    begin:push ax
          push cx
          push dx
          push ds
          push es
          push si
          push di

          pushf
          pop ax
          or ax,0100h
          push ax
          popf
          mov ax,cs
          mov ds,ax
          mov si,0202h

          mov ax,0b800h
          mov es,ax
          mov di,12*160+32*2
        s:mov cl,[si]
          mov ch,0
          jcxz ok
          mov ch,42h
          mov es:[di],cx
          add di,2
          inc si
          jmp short s
          nop
       ok:mov di,sp       ;恢复了其他寄存器，这时候栈顶才是要返回的地址，但是这又破坏了要保护的寄存器。。
          add di,14       ;直接根据保护的寄存器数目定位到要找的返回地址
          mov si,ss:[di]  ;修改之前div处的指令为两个nop，使div引起的中断可以返回到原程序
          mov ax,ss:[di+2]
          mov es,ax
          mov es:[si],9090h
          pop di
          pop si
          pop es
          pop ds
          pop dx
          pop cx
          pop ax


          iret

       ;ok:mov ax,4c00h
         ; int 21h
     over:nop
code ends
end start
