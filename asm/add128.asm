assume cs:code,ds:data,ss:stack
data segment
      dw 12,34,56,78,55,94,60,38
      dw 97,163,85,69,302,4226,89,650
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
         mov si,0
         mov di,16
         call add128

         mov ax,4c00h
         int 21h

   add128:push ax
          push cx
          push si
          push di

          sub ax,ax ;cf«Â¡„
          mov cx,8
        s:mov ax,[di]
          adc [si],ax
          inc si
          inc si
          inc di
          inc di
          loop s

          pop di
          pop si
          pop cx
          pop ax
          ret
code ends
end start
