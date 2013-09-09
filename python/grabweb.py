#!/usr/bin/python

from urllib import urlretrieve

def firstNoneBlank(lines):
	for el in lines:
		if not el.strip():
			continue
		else:
			return el

def fllines(webpage):
	f = open(webpage)
	lines = f.readlines()
	f.close()
	print firstNoneBlank(lines),
	lines.reverse()
	print firstNoneBlank(lines),

def prtall(wbp):
	f = open(wbp)
	lines = f.readlines()
	f.close()
	print lines

def dld(url='http://www',process=fllines):
	try:
		rt = urlretrieve(url)[0]
	except IOError,e:
		print e
		rt = None
	if rt:
		process(rt)

if __name__ == '__main__':
	tograb = raw_input('enter the url you want to grab : ')
	dld(url=tograb,process=prtall)
