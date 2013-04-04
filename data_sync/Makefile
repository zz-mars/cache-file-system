CC = gcc
CC_FLAGS = -g -o
LINK_LIBS = -lcrypto
OBJS = md5.o s_sync.o c_sync.o
All = s_sync c_sync
.PHONY : all s_sync c_sync
all : s_sync c_sync
s_sync : md5.o s_sync.o
	$(CC) md5.o s_sync.o $(CC_FLAGS) s_sync $(LINK_LIBS)
c_sync : md5.o c_sync.o
	$(CC) md5.o c_sync.o $(CC_FLAGS) c_sync $(LINK_LIBS)
.PHONY : clean
clean :
	rm	$(OBJS) s_sync c_sync
