assume cs:code
code segment
      mov ax,0020h
      mov ss,ax
      mov sp,000Ch
      mov ax,0ffffh
      mov ds,ax
      mov bx,000Ah
      mov cx,6
    s:push [bx]
      dec bx
      dec bx
      loop s
      mov ax,4c00h
      int 21h
code ends
end
