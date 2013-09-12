BINS = sort_test select_median
.PHONY : all
all : sort_test select_median

sort_test : sort.o sort_test.o
	gcc $? -o $@

select_median : select_median.o sort.o
	gcc $? -o $@

.PHONY : clean
clean :
	-rm *.o $(BINS)
