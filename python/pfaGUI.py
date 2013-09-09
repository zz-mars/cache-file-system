#!/usr/bin/python

from functools import partial
import Tkinter

root = Tkinter.Tk()

Mybutton = partial(Tkinter.Button,root,fg='white',bg = 'blue')
b1 = Mybutton(text='BUTTON1')
b2 = Mybutton(text='BUTTON2')
qb = Mybutton(text='QUIT',bg='red',command=root.quit)
b1.pack()
b2.pack()
qb.pack(fill=Tkinter.X,expand=True)
root.title('PFAs')
root.mainloop()
