#!/usr/bin/python

stack = []

def pushit():
	stack.append(raw_input('Enter new element: ').strip())

def popit():
	if len(stack) == 0:
		print "Cannot pop from an empty stack!"
	else:
		print stack.pop(),' pop out from stack'

def viewstack():
	print stack

def showmenu():
	print '''
1) p(U)sh
2) p(O)p
3) (V)iew
4) (S)how menu
4) (Q)uit
Please enter choice: 
'''
cmds = {'u':pushit,'o':popit,'v':viewstack,'s':showmenu}

def my_main():

	showmenu()

	while True:

		while True:

			try:
				cmd = raw_input().strip()[0].lower()
			except (EOFError,KeyboardInterrupt,IndexError),e:
				print e
				cmd = 'q'

			if cmd not in 'uovsq':
				print "invalid command"
				cmd = 'q'
			break

		if cmd == 'q':
			break

		cmds[cmd]()

if __name__ == '__main__':
	my_main()

