CC_FLAGS = -Wall -pedantic -std=c99 -o
BINS = bp_manual bp_use_lib print

.PHONY : all
all : bp_use_lib bp_manual print
bp_manual : bp_manual.o debuglib.o
	gcc $? $(CC_FLAGS) $@

bp_use_lib : bp_use_lib.o debuglib.o
	gcc $? $(CC_FLAGS) $@

print : print.o
	gcc $? -o $@

.PHONY : clean
clean :
	-rm *.o $(BINS)
