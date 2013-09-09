#!/usr/bin/python

import os,socket,errno,types,tempfile

class NetworkError(IOError):
	pass

class FileError(IOError):
	pass

def updArgs(args,newarg=None):
	if isinstance(args,IOError):
		myargs = []
		myargs.extend([arg for arg in args])
	else:
		myargs = list(args)
	
	if newarg:
		myargs.append(newarg)

	return tuple(myargs)

def fileArgs(f,mode,args):
	if args[0] == errno.EACCES and \
				'access' in dir(os):
		perms = ''

		permd = {'r':os.R_OK,'w':os.W_OK,'x':os.X_OK}

		for ep in 'rwx':
			if os.access(f,permd[ep]):
				perms += ep
			else:
				perms += '-'

		if isinstance(args,IOError):
			myargs = []
			myargs.extend([arg for arg in args])
		else:
			myargs = list(args)

		myargs[1] = "'%s' %s (perm: '%s')" % (mode,myargs[1],perms)
	
		myargs.append(args.filename)

	else:
		myargs = args

	return tuple(myargs)

def myconnect(sock,host,port):
	try:
		sock.connect((host,port))
	except socket.error,args:
		myargs = updArgs(args)
		if len(myargs) == 1:
			myargs = (errno.ENXIO,myargs[0])
			raise NetworkError,updArgs(myargs,host + ':' + str(port))

def myopen(f,mode='r'):
	try:
		fo = open(f,mode)
	except IOError,args:
		raise FileError,fileArgs(f,mode,args)
	return fo
	
def testfile():
	fn = tempfile.mktemp()
	f = open(fn,'w')
	f.close()

	for et in ((0,'r'),(0100,'r'),(0400,'w'),(0500,'w')):
		try:
			os.chmod(fn,et[0])
			f = myopen(fn,et[1])
		except FileError,args:
			print "%s: %s" % (args.__class__.__name__,args)
		else:
			print file,"open ok"
			
	os.chmod(fn,0777)
	os.unlink(fn)

def testnet():
	s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)

	for eh in ('deli','www','localhost'):
		try:
			myconnect(s,eh,8080)
		except NetworkError,args:
			print "%s: %s" % (args.__class__.__name__,args)

if __name__ == '__main__':
	testfile()
	testnet()


