assume cs:code,ds:data
data segment
     db "Seginner's All-purpose Symbolic Instruction Code.",0
data ends
code segment
    start:mov ax,data
          mov ds,ax

          mov si,0
          call letterc

          mov ax,4c00h
          int 21h

  letterc:push ax
          push si

        s:mov al,[si]
          cmp al,0
          je ok
          cmp al,'a'
          jb next
          cmp al,'z'
          ja next
          and al,11011111b
          mov [si],al
     next:inc si
          jmp short s
       ok:pop si
          pop ax
          ret

code ends
end start
