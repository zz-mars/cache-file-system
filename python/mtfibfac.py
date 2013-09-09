#!/usr/bin/python

from MyThread import MyThread
from time import ctime,sleep

def fib(x):
	sleep(0.005)
	if x < 2:return 1
	return fib(x-2) + fib(x-1)

def fac(x):
	sleep(0.1)
	if x < 2:return 1
	return (fac(x-1) * x)

def sum(x):
	sleep(0.1)
	if x < 2:return 1
	return (sum(x-1) + x)
	
funcs = [fib,fac,sum]
n = 12

def main():
	print 'single thread...'
	for func in funcs:
		print func.__name__,'start@',ctime()
		print func(n)
		print func.__name__,'end@',ctime()

	threads = []

	for i in range(len(funcs)):
		mythread = MyThread(funcs[i],(n,),funcs[i].__name__)
		threads.append(mythread)

	print 'multi-threads...'
	for th in threads:
		th.start()

	for th in threads:
		th.join()
		print th.getResult()

	print 'main--done'

if __name__ == '__main__':
	main()
