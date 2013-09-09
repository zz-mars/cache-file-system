#!/usr/bin/python
from MyThread import MyThread
from Queue import Queue
from time import sleep
from random import randint

def writeq(q):
	print 'put item to q...'
	q.put('xxx',1)
	print 'after put,qsize : ',q.qsize()

def readq(q):
	print 'get item from q...'
	q.get(1)
	print 'after get,qsize : ',q.qsize()

def producer(q,n):
	for i in range(n):
		writeq(q)
		sleep(randint(1,3))

def consumer(q,n):
	for i in range(n):
		readq(q)
		sleep(randint(2,5))

funcs = [producer,consumer]

def main():
	q = Queue(32)
	threads = []

	for func in funcs:
		th = MyThread(func,(q,randint(2,5)),func.__name__)
		threads.append(th)

	for th in threads:
		th.start()

	for th in threads:
		th.join()

	print 'All Done'

if __name__ == '__main__':
	main()

