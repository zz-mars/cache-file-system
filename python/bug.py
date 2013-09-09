#!/usr/bin/python
'bug'
num_str = raw_input('Enter a number:')

try:
	num = int(num_str)
except ValueError,e:
	print e
finally:
	fac_list = range(1,num+1)

	print fac_list

	i = 0

	l_len = len(fac_list)

	while i < len(fac_list):
		if num % fac_list[i] == 0:
			del fac_list[i]
		else:
			i += 1

	print fac_list

