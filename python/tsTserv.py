#!/usr/bin/python

from socket import *
from time import ctime

HOST = ''
PORT = 21567
BUFSIZ = 1024
ADDR = (HOST,PORT)

tcpServSocket = socket(AF_INET,SOCK_STREAM)
tcpServSocket.bind(ADDR)
tcpServSocket.listen(5)

while True:

	print 'waiting for connection...'
	tcpCliSock,addr = tcpServSocket.accept()
	print 'connection from :',addr

	while True:

		data = tcpCliSock.recv(BUFSIZ)
		if not data:
			break
		tcpCliSock.send('[%s] %s' % (ctime(),data))

	tcpCliSock.close()

tcpServSocket.close()
