'check if the input is a key word'
import string,keyword

alphas = string.letters + '_'
nums = string.digits
keywordlist = keyword.kwlist

if __name__ == '__main__':
	while True:
		try:
			inp = raw_input("Enter a name('-' to exit) : ")
		except (EOFError,KeyboardInterrupt,IndexError),e:
			print e
			break
		if inp[0] == '-':
			print "exit"
			break
		if inp[0] not in alphas:
			print 'not start with a letter or underscore'
			continue
		if inp not in keywordlist:
			print "%s is not a key word!" % inp
		else:
			print "%s is a key word!" % inp
