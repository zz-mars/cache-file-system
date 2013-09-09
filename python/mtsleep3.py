#!/usr/bin/python
from time import ctime,sleep
import threading

def loop(nl,nsec):
	print 'loop:',nl,'now start@',ctime(),'will sleep for ',nsec,'seconds'
	sleep(nsec)
	print 'loop:',nl,'over'

def main():
	threads = []
	for i in range(1,4):
		new_thread = threading.Thread(target=loop,args=(i-1,i*2))
		threads.append(new_thread)

	for i in range(len(threads)):
		threads[i].start()

	for i in threads:
		i.join()

	print '--- main --- done!'

if __name__ == '__main__':
	main()
