assume cs:code,ds:data
code segment
       mov bx,0
       mov ax,[bx]
       mov ax,4c00h
       int 21h
code ends
data segment
       dw 0123h,0456h
data ends
end
