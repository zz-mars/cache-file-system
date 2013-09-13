BINS = redBlackTreeTest

redBlackTreeTest : redBlackTreeTest.o redBlackTree.o
	gcc $? -o $@

.PHONY : clean

clean :
	-rm $(BINS) *.o
