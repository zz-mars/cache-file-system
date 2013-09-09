#!/usr/bin/python

import thread
from time import sleep,ctime

def loop0():
	print 'loop0 start ==- ',ctime()
	sleep(4)
	print 'loop0 end ==- ',ctime()

def loop1():
	print 'loop1 start ==- ',ctime()
	sleep(2)
	print 'loop1 end ==- ',ctime()

def main():
	print 'start thread 0:',ctime()
	thread.start_new_thread(loop0,())
	print 'start thread 1:',ctime()
	thread.start_new_thread(loop1,())
	sleep(6)
	print 'all done',ctime()


if __name__ == '__main__':
	main()
