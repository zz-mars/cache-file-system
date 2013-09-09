#!/usr/bin/python

from time import ctime,sleep

def tsfunc(func):
	def generalWrapper():
		print '[%s] : [%s] called' % (
				ctime(),func.__name__	)
		func()
		print '[%s] : [%s] finished' % (
				ctime(),func.__name__	)
		return

	return generalWrapper

@tsfunc
def foo():
	print 'foo'

foo()
sleep(2)

for i in range(2):
	foo()
	sleep(1)

