assume cs:code,ds:data,ss:stack
data segment
      db '1.display0000000'
      db '2.brows000000000'
      db '3.replace0000000'
      db '4.modify00000000'
data ends
stack segment
      dw 0,0,0,0,0,0,0,0
stack ends
code segment
start:mov ax,data
      mov ds,ax

      mov ax,stack
      mov ss,ax
      mov sp,10h

      mov bx,2
      mov cx,4
      mov al,11011111b
   s0:push cx
      mov cx,3
      mov si,0
   s1:and [bx][si],al
      inc si
      loop s1
      add bx,16
      pop cx
      loop s0

      mov ax,4c00h
      int 21h
code ends
end start
