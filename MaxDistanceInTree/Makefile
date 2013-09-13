BINS = MaxDistanceInTree MaxDistanceInTree_cc MaxDistanceInMultWayTree

.PHONY : all
all : MaxDistanceInTree MaxDistanceInTree_cc MaxDistanceInMultWayTree
MaxDistanceInTree : MaxDistanceInTree.o
	gcc $? -o $@

MaxDistanceInTree_cc : MaxDistanceInTree_cc.o
	g++ $? -o $@

MaxDistanceInMultWayTree : MaxDistanceInMultWayTree.o
	gcc $? -o $@


.PHONY : clean
clean :
	-rm $(BINS) *.o
