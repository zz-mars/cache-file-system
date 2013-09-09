#!/usr/bin/python
'''
encode and decode a unicode file
'''
CODECS = 'utf-8'
file = 'unicode.txt'
ho = u'Hello,world\n'
bo = ho.encode(CODECS)
f = open(file,'w')
f.write(bo)
f.close()

f = open(file,'r')
bi = f.read()
f.close()
hi = bi.decode(CODECS)
print hi,
