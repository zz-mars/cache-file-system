BINS = sccTest

CC_FLAGS = -g -o

.PHONY : all
all : sccTest
sccTest : sccTest.o scc.o
	gcc $? $(CC_FLAGS) $@

.PHONY : clean
clean :
	-rm $(BINS) *.o

