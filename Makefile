CFLAGS := -Wall -Wextra -Wpedantic -Wfloat-equal -Wvla -std=c99
# CFLAGS := -Wall -Wextra -Wpedantic -Wfloat-equal -Wvla -std=c99 -Werror -O2 -ggdb3
SANI := -fsanitize=undefined -fsanitize=address
NLBS := $(wildcasrd *.nlb)
RESULTS := $(NLBS:.nlb=.result)

# all: testexact spellexact
all: parse parse_s parse_v interp interp_s interp_v extension extension_s extension_v


extension: nlab.c nlab.h main.c stack.c stack.h test.c extension.c extension.h
	clang nlab.c main.c stack.c test.c extension.c $(CFLAGS) -O2 -DINTERP -DEXTENSION -o extension -lm

interp: nlab.c nlab.h main.c stack.c stack.h test.c
	clang nlab.c main.c stack.c test.c $(CFLAGS) -O2 -DINTERP -o interp -lm

parse: nlab.c nlab.h main.c stack.c stack.h test.c
	clang nlab.c main.c stack.c test.c $(CFLAGS) -O2 -o parse -lm



# For all .nlb files, run them and output result to a .result file
runall : $(RESULTS)

%.result:
		./extension $*.nlb > $*.results

clean:
		rm -f parse parse_s parse_v interp interp_s interp_v extension extension_s extension_v $(RESULTS)
