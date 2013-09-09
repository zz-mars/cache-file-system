#!/usr/bin/python

def convert(func,seq):
	return [func(en) for en in seq]

myseq = [123,45.67,-6.2e8,999999999L]

print convert(int,myseq)
print convert(float,myseq)
print convert(long,myseq)

