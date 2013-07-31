assume cs:code,ds:data
data segment
     dw 0123h,0456h,0789h,0abch,0defh,0fedh,0cbah,0987h
data ends
code segment
start:mov ax,data
      mov ds,ax
      mov bx,0
      mov cx,8
      mov ax,0
    s:add ax,[bx]
      add bx,2
      loop s
      mov ax,4c00h
      int 21h
code ends
end start

