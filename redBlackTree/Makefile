BINS = redBlackTreeTest
CC_FLAGS = -g -o

redBlackTreeTest : redBlackTreeTest.o redBlackTree.o
	gcc $? $(CC_FLAGS) $@

redBlackTree.o: redBlackTree.c redBlackTree.h queue.h
redBlackTreeTest.o: redBlackTreeTest.c redBlackTree.h

.PHONY : clean

clean :
	-rm $(BINS) *.o
