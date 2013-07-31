assume cs:code
code segment
    start:mov ax,0
          mov es,ax
          mov di,200h

          mov ax,cs
          mov ds,ax
          mov si,offset int19
          mov cx,offset int19_e - offset int19

          mov ax,offset rst - offset int19
          add ax,200h
          mov [si+2],ax
          mov ax,offset stt - offset int19
          add ax,200h
          mov [si+4],ax

          mov ax,offset showt - offset int19
          add ax,200h
          mov [si+6],ax
          mov ax,offset sett - offset int19
          add ax,200h
          mov [si+8],ax

          cld
          rep movsb

          cli
          mov word ptr es:[19h*4],200h
          mov word ptr es:[19h*4+2],0
          sti

          mov ax,4c00h
          int 21h

    int19:jmp short s
       dw 0,0,0,0   ;存储子程序地址
       dw 0,0ffffh  ;第一个子程序需要转到的地址
       dw 7c00h,0   ;第二个子程序需要的地址
        s:push bx
          push ds

          mov bl,ah
          mov bh,0
          add bx,bx

          mov ax,cs
          mov ds,ax
          call word ptr [bx+202h]

          pop ds
          pop bx
          iret

      rst:jmp dword ptr ds:[20ah]
          ret
      stt:mov ax,0
          mov es,ax
          mov bx,7c00h
          mov al,1
          mov ch,0
          mov cl,1
          mov dh,0
          mov dl,80h
          mov ah,2
          int 13h
          jmp dword ptr ds:[20eh]
          ret

    showt:jmp short showst
          db 9,8,7,4,2,0
          db '// ::',0
          dw  0,0       ;存储原先9号中断处理程序的段地址和偏移地址
          dw  0         ;esc后，跳出循环后的地址
   showst:push ax       ;显示时间的子程序需要实现对F1键和esc键的相应动作，需要在此子程序中
          push bx       ;重新设定9号键盘中断处理程序。此程序结束后要恢复原先的中断处理程序
          push cx
          push si
          push di
          push ds
          push es

          mov ax,cs  ;cs=0
          mov ds,ax  ;ds=0
          mov ax,0b800h
          mov es,ax
          mov bh,42h
          mov di,720h
          mov si,offset showt- offset int19
          add si,202h

          mov ax,si
          add ax,12
          mov bp,ax    ;bp用于寻址原先的9号中断处理程序的地址

          push ds:[36]    ;保存原先9号中断处理程序的入口地址
          pop ds:[bp]
          push ds:[38]
          pop ds:[bp+2]

          push cs     ;设置新的9号中断处理程序地址
          pop ds:[38]
          mov ax,offset int9 - offset int19
          add ax,200h
          mov ds:[36],ax

          mov ax,offset showover - offset int19
          add ax,200h
          mov ds:[bp+4],ax        ;按下esc后跳转出来的地址

          pushf            ;打开IF标志，因为此时是在19h中断中，if=0，后面键盘中断无法响应
          pop ax
          or ax,0200h
          push ax
          popf

 refresht:mov si,bp    ;出现问题了，对于按下F1键，没问题，因为程序处理完会返回到中断的位置继续执行。
          sub si,12
          mov di,720h    ;而按下esc键却要跳转到一个新位置执行。采用的方法是直接在栈内修改iret执行返回的地址IP
          mov cx,6  ;但是看下被键盘中断的程序段，循环显示时间的程序中有push si和push di的栈操作
      lpt:mov al,[si] ;如果压栈结束被esc中断，我们确实可以跳到showover处执行。但接下来刚才压入的si和di并未
          out 70h,al ;出栈，而是被当作原先栈中保存的内容出栈。显然会引起错误，程序也无法正确回到int19的主程序中
          in al,71h  ;简单说来，就是push和pop有很大可能不配对。解决方法：不使用栈来保存si和di。
          push ax
          push cx
          mov cl,4
          shr al,cl
          pop cx
          add al,30h
          mov bl,al
          mov es:[di],bx
          pop ax
          and al,0fh
          add al,30h
          mov bl,al
          mov es:[di+2],bx

          mov bl,[si+6]
          mov es:[di+4],bx

          inc si
          add di,6
          loop lpt
          jmp short refresht

 showover:pushf         ;显示时间程序结束，重新关闭IF
          pop ax
          and ax,0fdffh
          push ax
          popf
          pop es
          pop ds
          pop di
          pop si
          pop cx
          pop bx
          pop ax

          ret

    int9:push ax
         push bp
         in al,60h

         pushf
         call dword ptr ds:[bp]

         cmp al,3bh  ;F1键
         je sf1
         cmp al,1    ;esc键
         je sesc
         jmp short int9ok
     sf1:inc bh
         jmp short int9ok
    sesc:push ds:[bp]
         push ds:[bp+2]
         pop ds:[38]
         pop ds:[36]

         mov ax,ds:[bp+4]

         mov bp,sp
         mov [bp+4],ax
  int9ok:pop bp
         pop ax
         iret

     sett:ret

  int19_e:nop

code ends
end start
