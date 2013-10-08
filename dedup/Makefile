BINS = dedup

dedup : dedup.o md5.o
	gcc $? -g -o $@

.PHONY : clean
clean :
	-rm -rf $(BINS) *.o

