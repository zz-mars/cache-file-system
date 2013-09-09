#!/usr/bin/python
'read text file'

#get file name

fn = raw_input('file to read : ')
try:
	fobj = open(fn,'r')
except IOError,e:
	print "file open error :",e
else:
	for eachline in fobj:
		print eachline,
	fobj.close()
