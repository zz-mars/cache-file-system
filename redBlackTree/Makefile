BINS = redBlackTreeTest
CC_FLAGS = -g -o

redBlackTreeTest : redBlackTreeTest.o redBlackTree.o
	gcc $? $(CC_FLAGS) $@

.PHONY : clean

clean :
	-rm $(BINS) *.o
