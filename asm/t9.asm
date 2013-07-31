assume cs:code,ds:data,ss:stack
data segment
       db 'welcome to masm!'
       db 42h,0c2h,71h
data ends
stack segment
       db 16 dup (0)
stack ends
code segment
   start:mov ax,data
         mov ds,ax

         mov ax,0b800h
         mov es,ax

         mov ax,stack
         mov ss,ax
         mov sp,16


         mov cx,3
         mov bx,0
         mov bp,0

      s0:push cx
         mov cx,16
         mov si,0
         mov di,0
         mov ah,ds:[bp].16
      s1:mov al,[si]
         mov es:[bx].720h[di],ax
         inc si
         add di,2
         loop s1

         add bx,160
         inc bp
         pop cx
         loop s0

         mov ax,4c00h
         int 21h
code ends
end start
