try:
	fn = raw_input('enter a name :')
	fo = open(fn,'r')
	for e in fo:
		print e,
	fo.close()
except IOError,e:
	print 'file open error:',e
