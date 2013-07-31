assume cs:code,ds:data,ss:stack
data segment
     db 'Welcome to masm!',0
data ends
stack segment
     db 32 dup (0)
stack ends
code segment
    start:mov ax,stack
          mov ss,ax
          mov sp,32

          mov ax,data
          mov ds,ax
          mov si,0
          mov dh,10
          mov dl,15
          mov cl,42h
          int 7ch

          mov ax,4c00h
          int 21h
code ends
end start
