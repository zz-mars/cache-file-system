#!/usr/bin/python
'user register and login'
db = {}

def newuser():
	while True:
		prompt = 'create a user name: '
		uname = raw_input(prompt)
		if db.has_key(uname):
			prompt = 'name taken!try a new one: '
			continue
		else:
			break

	pwd = raw_input("password : ")
	db[uname] = pwd

def olduser():
	uname = raw_input("login: ")
	pwd = raw_input("password: ")
	password = db.get(uname)
	if password == pwd:
		print "welcome back"
	else:
		print "login fail!"

def mmain():

	print '''
	1) (N)ew user
	2) (O)ld user
	3) (E)xit
	please select : 
'''

	while True:
		cmd = raw_input().strip().lower()
		if cmd not in 'noe':
			cmd = 'e'
		if cmd == 'n':
			newuser()
			continue
		if cmd == 'o':
			olduser()
			continue
		if cmd == 'e':
			break

if __name__ == '__main__':
	mmain()
