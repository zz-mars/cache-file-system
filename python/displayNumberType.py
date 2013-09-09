#!/usr/bin/python

def displayNumberType(num):
	print num,'is',
	if isinstance(num,(int,long,float,complex)):
		print 'a number of type:',type(num).__name__
	else:
	 print 'not a number at all!'

displayNumberType(-69);
displayNumberType(9999999999999999L);
displayNumberType(98.4);
displayNumberType(-3.6+0.9j);
displayNumberType('xxxxx');
