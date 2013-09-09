#!/usr/bin/python
from time import ctime,sleep
import threading

def loop(nl,nsec):
	print 'loop:',nl,'now start@',ctime(),'will sleep for ',nsec,'seconds'
	sleep(nsec)
	print 'loop:',nl,'over'

class MyThread(threading.Thread):
	def __init__(self,func,args):
		super(MyThread,self).__init__()
		self.func = func
		self.args = args

	def run(self):
		apply(self.func,self.args)

def main():
	threads = []
	for i in range(1,4):
		new_thread = MyThread(loop,(i-1,i*2))
		threads.append(new_thread)

	for i in threads:
		i.start()

	for i in threads:
		i.join()

	print '--- main --- done!'

if __name__ == '__main__':
	main()
