assume cs:code,ds:data
data segment
     table  dw ag0,ag30,ag60,ag90,ag120,ag150,ag180
     ag0    db '0.0',0
     ag30   db'0.5',0
     ag60   db '0.866',0
     ag90   db '1',0
     ag120  db '0.866',0
     ag150  db '0.5',0
     ag180  db '0.0',0
     errmsg db 'fail to get sin result for this given angle!',0
data ends
code segment
    start:mov ax,data
          mov ds,ax

          mov ax,182
          call showsin

          mov ax,4c00h
          int 21h

  showsin:push bx
          mov bl,30
          div bl
          cmp ah,0
          jne err
          mov bx,ax
          add bx,bx
          mov bx,[bx]
          jmp short msg
      err:mov bx,offset errmsg
      msg:call showmsg
          pop bx
          ret

  showmsg:push ax
          push es
          push si
          push di

          mov di,160*12+60
          mov si,0
          mov ax,0b800h
          mov es,ax
          mov ah,42h
        s:mov al,[bx][si]
          cmp al,0
          je ok
          mov es:[di],ax
          inc si
          add di,2
          jmp short s
       ok:pop di
          pop si
          pop es
          pop ax
          ret

code ends
end start
