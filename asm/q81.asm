assume cs:code,ds:data
data segment
        dd 100001
        dw 100
        dw 0
data ends
code segment
  start:mov ax,data
        mov ds,ax

        mov dx,ds:[0]
        mov ax,ds:[2]
        mov bx,ds:[4]

        div bx
        mov ds:[5],ax

        mov ax,4c00h
        int 21h
code ends
end start
