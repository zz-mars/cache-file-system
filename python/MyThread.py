#!/usr/bin/python
import threading
from time import ctime

class MyThread(threading.Thread):
	def __init__(self,func,args,name=''):
		super(MyThread,self).__init__()
		self.func = func
		self.args = args
		self.name = name

	def run(self):
		print self.name,':start@',ctime()
		self.res = apply(self.func,self.args)
		print self.name,':end@',ctime()

	def getResult(self):
		return self.res
