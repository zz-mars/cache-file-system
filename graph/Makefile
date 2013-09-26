BINS = graphTest

CC_FLAGS = -g -o

.PHONY : all
all : graphTest
graphTest : graphTest.o graph.o
	gcc $? $(CC_FLAGS) $@

.PHONY : clean
clean :
	-rm $(BINS) *.o

