#!/usr/bin/python

import thread
from time import ctime,sleep

def loop(nl,nsec,lock):
	print 'loop %d start @%s' % (nl,ctime())
	sleep(nsec)
	print 'loop %d end @%s' % (nl,ctime())
	lock.release()

def main():
	locks = []
	for i in range(1,4):

		lock = thread.allocate_lock()
		lock.acquire()
		locks.append(lock)

		thread.start_new_thread(loop,(i,2*i,lock))
	
	for i in range(3):
		while locks[i].locked():pass

	print 'all done',ctime()

if __name__ == '__main__':
	main()
