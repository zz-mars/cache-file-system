#/usr/bin/python

class AnyIter(object):
	def __init__(self,data,safe=True):
		self.safe = safe
		self.iter = iter(data)

	def next(self,howmany=1):
		rt = []
		for i in range(howmany):
			try:
				rt.append(self.iter.next())
			except StopIteration:
				if self.safe:
					break
				else:
					raise
		return rt
