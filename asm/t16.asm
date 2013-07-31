assume cs:code,ss:stack
stack segment
      dw 64 dup (0)
stack ends
code segment
   start:mov ax,stack
         mov ss,ax
         mov sp,128

         mov ah,0
         mov al,00000010b
         int 7ch

         mov ax,4c00h
         int 21h
code ends
end start
