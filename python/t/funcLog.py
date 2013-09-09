#!/usr/bin/python

from time import time

def logged(when):

	def log(f,*argt,**argd):
		print '''called
function : %s
args : %r
argd : %r''' % (f,argt,argd)

	def preLogged(f):
		def wrapper(*argt,**argd):
			log(f,*argt,**argd)
			return f(*argt,**argd)
		return wrapper

	def postLogged(f):
		def wrapper(*argt,**argd):
			now = time()
			try:
				return f(*argt,**argd)
			finally:
				log(f,*argt,**argd)
				print "time delta : %s" % (time()-now)
		return wrapper

	try:
		return {'prev':preLogged,'post':postLogged}[when]
	except KeyError,e:
		raise ValueError(e),'must be prev or post'

@logged('post')
def hello(name):
	print 'hello,',name

hello('world')

@logged('prev')
def hello2(name):
	print 'hell2,',name

hello2('world')
