assume cs:code,ds:data,ss:stack
data segment
   table dw hint,reset,boot,clock,set_c
   hint db 'pls select the program you want to run','$'
   reset db 'reset pc ------- 0','$'
   boot  db 'start system ----1','$'
   clock db 'clock -----------2','$'
   set_c db 'set clock -------3','$'
data ends
stack segment
     dw 64 dup (0)
stack ends
code segment
    start:mov ax,stack
          mov ss,ax
          mov sp,128

          mov ax,data
          mov ds,ax

          mov ax,0b800h
          mov es,ax

          mov bh,0
          mov dh,6
          mov dl,16
          mov cx,5
          mov bp,0

        s:push dx
          mov ah,2  ;置光标
          int 10h

          mov dx,table[bp]  ;显示字符串
          mov ah,9
          int 21h

          pop dx
          inc dh
          add bp,2
          loop s

          mov ah,2
          int 10h   ;置光标，等待输入

          push dx   ;计算输入字符偏移地址的时候会修改dx，先保护之，供后面使用
          mov al,160
          mul dh
          mov dh,0
          add dx,dx
          add ax,dx
          mov di,ax
          pop dx

    getch:mov ah,0
          int 16h
          cmp ah,0bh
          je rst
          cmp ah,2
          je stt
          cmp ah,3
          je showt
          cmp ah,4
          je sett
          jmp short getch
      rst:mov ah,0
          jmp short ok
      stt:mov ah,1
          jmp short ok
    showt:mov ah,2
          jmp short ok
     sett:mov ah,3
       ok:push ax
          mov ah,42h
          mov es:[di],ax
      e_b:mov ah,0
          int 16h
          cmp ah,1ch  ;回车
          je int19
          cmp ah,0eh  ;退格
          jne short e_b
          mov ah,2
          int 10h
          mov al,0
          mov ah,7
          mov es:[di],ax
          pop ax
          jmp short getch

    int19:pop ax
          int 19h

          mov ax,4c00h
          int 21h
code ends
end start
