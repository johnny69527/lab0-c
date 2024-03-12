CC = gcc
CFLAGS = -O1 -g -Wall -Werror -Idudect -I.

# Emit a warning should any variable-length array be found within the code.
CFLAGS += -Wvla

GIT_HOOKS := .git/hooks/applied
DUT_DIR := dudect
all: $(GIT_HOOKS) qtest

tid := 0

# Control test case option of valgrind
ifeq ("$(tid)","0")
    TCASE :=
else
    TCASE := -t $(tid)
endif

# Control the build verbosity
ifeq ("$(VERBOSE)","1")
    Q :=
    VECHO = @true
else
    Q := @
    VECHO = @printf
endif

# Enable sanitizer(s) or not
ifeq ("$(SANITIZER)","1")
    # https://github.com/google/sanitizers/wiki/AddressSanitizerFlags
    CFLAGS += -fsanitize=address -fno-omit-frame-pointer -fno-common
    LDFLAGS += -fsanitize=address
endif

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

OBJS := qtest.o report.o console.o harness.o queue.o \
        random.o dudect/constant.o dudect/fixture.o dudect/ttest.o \
        shannon_entropy.o \
        linenoise.o web.o list_sort.o quick_sort.o timsort.o

deps := $(OBJS:%.o=.%.o.d)

qtest: $(OBJS)
	$(VECHO) "  LD\t$@\n"
	$(Q)$(CC) $(LDFLAGS) -o $@ $^ -lm

%.o: %.c
	@mkdir -p .$(DUT_DIR)
	$(VECHO) "  CC\t$@\n"
	$(Q)$(CC) -o $@ $(CFLAGS) -c -MMD -MF .$@.d $<

check: qtest
	./$< -v 3 -f traces/trace-eg.cmd

test: qtest scripts/driver.py
	scripts/driver.py -c

valgrind_existence:
	@which valgrind 2>&1 > /dev/null || (echo "FATAL: valgrind not found"; exit 1)

valgrind: valgrind_existence
	# Explicitly disable sanitizer(s)
	$(MAKE) clean SANITIZER=0 qtest
	$(eval patched_file := $(shell mktemp /tmp/qtest.XXXXXX))
	cp qtest $(patched_file)
	chmod u+x $(patched_file)
	sed -i "s/alarm/isnan/g" $(patched_file)
	scripts/driver.py -p $(patched_file) --valgrind $(TCASE)
	@echo
	@echo "Test with specific case by running command:" 
	@echo "scripts/driver.py -p $(patched_file) --valgrind -t <tid>"

clean:
	rm -f $(OBJS) $(deps) *~ qtest /tmp/qtest.*
	rm -rf .$(DUT_DIR)
	rm -rf *.dSYM
	(cd traces; rm -f *~)

distclean: clean
	rm -f .cmd_history

linux_sort.dat: list_sort.o
	./qtest -v 2 -f traces/trace-perf-linux-sort.cmd -l linux_sort.dat
	cat linux_sort.dat | grep -v 'Delta time =' | sed 's/# //g' > linux_sort.dat01
	cat linux_sort.dat | grep 'Delta time =' | sed 's/Delta time = //g' > linux_sort.dat02
	pr -m -T linux_sort.dat01 linux_sort.dat02 > linux_sort.dat
	rm linux_sort.dat01 linux_sort.dat02

merge_sort.dat: queue.o
	./qtest -v 2 -f traces/trace-perf-merge-sort.cmd -l merge_sort.dat
	cat merge_sort.dat | grep -v 'Delta time =' | sed 's/# //g' > merge_sort.dat01
	cat merge_sort.dat | grep 'Delta time =' | sed 's/Delta time = //g' > merge_sort.dat02
	pr -m -T merge_sort.dat01 merge_sort.dat02 > merge_sort.dat
	rm merge_sort.dat01 merge_sort.dat02

quick_sort.dat: quick_sort.o
	./qtest -v 2 -f traces/trace-perf-quick-sort.cmd -l quick_sort.dat
	cat quick_sort.dat | grep -v 'Delta time =' | sed 's/# //g' > quick_sort.dat01
	cat quick_sort.dat | grep 'Delta time =' | sed 's/Delta time = //g' > quick_sort.dat02
	pr -m -T quick_sort.dat01 quick_sort.dat02 > quick_sort.dat
	rm quick_sort.dat01 quick_sort.dat02

tim_sort.dat: timsort.o
	./qtest -v 2 -f traces/trace-perf-tim-sort.cmd -l tim_sort.dat
	cat tim_sort.dat | grep -v 'Delta time =' | sed 's/# //g' > tim_sort.dat01
	cat tim_sort.dat | grep 'Delta time =' | sed 's/Delta time = //g' > tim_sort.dat02
	pr -m -T tim_sort.dat01 tim_sort.dat02 > tim_sort.dat
	rm tim_sort.dat01 tim_sort.dat02
	
sort_perf: linux_sort.dat merge_sort.dat quick_sort.dat tim_sort.dat
	gnuplot scripts/sort_perf.gp
	# rm linux_sort.dat merge_sort.dat quick_sort.dat
	xdg-open sort_perf.png

linux_sort_qwc.dat: list_sort.o
	./qtest -v 2 -f traces/trace-perf-linux-sort-qwc.cmd -l linux_sort_qwc.dat
	cat linux_sort_qwc.dat | grep -v 'Delta time =' | sed 's/# //g' > linux_sort_qwc.dat01
	cat linux_sort_qwc.dat | grep 'Delta time =' | sed 's/Delta time = //g' > linux_sort_qwc.dat02
	pr -m -T linux_sort_qwc.dat01 linux_sort_qwc.dat02 > linux_sort_qwc.dat
	rm linux_sort_qwc.dat01 linux_sort_qwc.dat02

merge_sort_qwc.dat: queue.o
	./qtest -v 2 -f traces/trace-perf-merge-sort-qwc.cmd -l merge_sort_qwc.dat
	cat merge_sort_qwc.dat | grep -v 'Delta time =' | sed 's/# //g' > merge_sort_qwc.dat01
	cat merge_sort_qwc.dat | grep 'Delta time =' | sed 's/Delta time = //g' > merge_sort_qwc.dat02
	pr -m -T merge_sort_qwc.dat01 merge_sort_qwc.dat02 > merge_sort_qwc.dat
	rm merge_sort_qwc.dat01 merge_sort_qwc.dat02

quick_sort_qwc.dat: quick_sort.o
	./qtest -v 2 -f traces/trace-perf-quick-sort-qwc.cmd -l quick_sort_qwc.dat
	cat quick_sort_qwc.dat | grep -v 'Delta time =' | sed 's/# //g' > quick_sort_qwc.dat01
	cat quick_sort_qwc.dat | grep 'Delta time =' | sed 's/Delta time = //g' > quick_sort_qwc.dat02
	pr -m -T quick_sort_qwc.dat01 quick_sort_qwc.dat02 > quick_sort_qwc.dat
	rm quick_sort_qwc.dat01 quick_sort_qwc.dat02

tim_sort_qwc.dat: timsort.o
	./qtest -v 2 -f traces/trace-perf-tim-sort-qwc.cmd -l tim_sort_qwc.dat
	cat tim_sort_qwc.dat | grep -v 'Delta time =' | sed 's/# //g' > tim_sort_qwc.dat01
	cat tim_sort_qwc.dat | grep 'Delta time =' | sed 's/Delta time = //g' > tim_sort_qwc.dat02
	pr -m -T tim_sort_qwc.dat01 tim_sort_qwc.dat02 > tim_sort_qwc.dat
	rm tim_sort_qwc.dat01 tim_sort_qwc.dat02
	
sort_perf_qwc: linux_sort_qwc.dat merge_sort_qwc.dat quick_sort_qwc.dat tim_sort_qwc.dat
	gnuplot scripts/sort_perf_qwc.gp
	# rm linux_sort.dat merge_sort.dat quick_sort.dat
	xdg-open sort_perf_qwc.png

-include $(deps)
