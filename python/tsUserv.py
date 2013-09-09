#!/usr/bin/python

from socket import *
from time import ctime

HOST = ''
PORT = 21567
BUFSIZ = 1024
ADDR = (HOST,PORT)

udpServSocket = socket(AF_INET,SOCK_DGRAM)
udpServSocket.bind(ADDR)

while True:

	print 'waiting for message...'
	data,addr = udpServSocket.recvfrom(BUFSIZ)
	print 'message : [%s] from : [%s]',(data,addr)
	udpServSocket.sendto('[%s] %s' % (ctime(),data),addr)

udpServSocket.close()
