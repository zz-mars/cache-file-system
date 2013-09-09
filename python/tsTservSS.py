#!/usr/bin/python

from SocketServer import (TCPServer as TCP,
		StreamRequestHandler as SRH)
from time import ctime

HOST = ''
PORT = 21567
ADDR = (HOST,PORT)

class MyReqHandler(SRH):
	def handle(self):
		print '...request from:',self.client_address
		self.wfile.write('[%s] %s' % (ctime(),
					self.rfile.readline()))
		
tcpServ = TCP(ADDR,MyReqHandler)

print 'waiting for connection ..'

tcpServ.serve_forever()
