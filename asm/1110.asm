assume cs:code,ds:data
data segment
     db 'welcome to masm!'
     db 16 dup (0)
data ends
code segment
    start:mov ax,data
          mov ds,ax
          add ax,1
          mov es,ax

          mov si,0
          mov di,0
          mov cx,8
          cld
          rep movsw

          mov ax,4c00h
          int 21h
code ends
end start
