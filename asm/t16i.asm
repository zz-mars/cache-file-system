assume cs:code
code segment
   start:mov ax,0
         mov es,ax
         mov di,200h

         mov ax,cs
         mov ds,ax
         mov si,offset int7ch

         mov ax,offset sub1 - offset int7ch
         add ax,200h
         mov [si+2],ax
         mov ax,offset sub2 - offset int7ch
         add ax,200h
         mov [si+4],ax
         mov ax,offset sub3 - offset int7ch
         add ax,200h
         mov [si+6],ax
         mov ax,offset sub4 - offset int7ch
         add ax,200h
         mov [si+8],ax

         mov cx,offset int7ch_e - offset int7ch

         cld
         rep movsb

         mov ax,200h
         mov es:[7ch*4],ax
         mov ax,0
         mov es:[7ch*4+2],ax

         mov ax,4c00h
         int 21h


  int7ch:jmp short begin
         dw 0,0,0,0
   begin:push bx
         push ds
         mov bx,cs
         mov ds,bx
         mov bl,ah
         mov bh,0
         add bx,bx
         call word ptr [bx].202h
         pop ds
         pop bx
         iret

    sub1:push bx
         push cx
         push es

         mov bx,0b800h
         mov es,bx
         mov bx,0
         mov cx,2000
      s1:mov byte ptr es:[bx],' '
         add bx,2
         loop s1

         pop es
         pop cx
         pop bx
         ret

    sub2:push bx
         push cx
         push es

         mov bx,0b800h
         mov es,bx
         mov bx,1
         mov cx,2000
      s2:and byte ptr es:[bx],11111000b
         or byte ptr es:[bx],al
         add bx,2
         loop s2

         pop es
         pop cx
         pop bx
         ret

    sub3:push bx
         push cx
         push es

         mov cl,4
         shl al,cl

         mov bx,0b800h
         mov es,bx
         mov bx,1
         mov cx,2000
      s3:and byte ptr es:[bx],10001111b
         or byte ptr es:[bx],al
         add bx,2
         loop s3

         pop es
         pop cx
         pop bx
         ret


    sub4:push bx
         push cx
         push es
         push si

         mov bx,0b800h
         mov es,bx

         mov cx,24
         mov bx,0
     s44:push cx
         mov cx,80
         mov si,0
      s4:push es:[bx].160[si]
         pop es:[bx][si]
         add si,2
         loop s4
         pop cx
         add bx,160
         loop s44

         pop si
         pop es
         pop cx
         pop bx
         ret

int7ch_e:nop

code ends
end start
