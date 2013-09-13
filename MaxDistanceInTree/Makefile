BINS = MaxDistanceInTree MaxDistanceInTree_cc

.PHONY : all
all : MaxDistanceInTree MaxDistanceInTree_cc
MaxDistanceInTree : MaxDistanceInTree.o
	gcc $? -o $@

MaxDistanceInTree_cc : MaxDistanceInTree_cc.o
	g++ $? -o $@


.PHONY : clean
clean :
	-rm $(BINS) *.o
