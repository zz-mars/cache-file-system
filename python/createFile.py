#!/usr/bin/python
'make a text file'
import os
ls = os.linesep

#get filename

fname = "default"

while True:
	fname = raw_input("file_name : ")
	if os.path.exists(fname):
		print "error:%s already exists!" % fname
	else:
		break

#get file content
all = []
print "\nEnter lines ('.' to quit)\n"

while True:
	entry = raw_input('>');
	if entry == '.':
		break;
	else:
		all.append(entry)

print all

# write to file
fobj = open(fname,'w')
fobj.writelines(['%s%s' % (x,ls) for x in all])
fobj.close()
print "Done!\n"
