# SPDX-License-Identifier: GPLv2-or-later
#
# Makefile
# Copyright Peter Jones <pjones@redhat.com>
#
default: all

TARGETS=analyzer-malloc-leak
LTO_TARGETS=$(foreach x,$(TARGETS),${x}.lto)
MAXTEST=2

DTEST0YNN = -DCALL_TEST0 -DDEFINE_TEST0
DTEST0YYN = -DCALL_TEST0 -DDEFINE_TEST0 -DDEFINE_TEST1
DTEST0YYY = -DCALL_TEST0 -DDEFINE_TEST0 -DDEFINE_TEST1 -DDEFINE_TEST2
DTEST0YNY = -DCALL_TEST0 -DDEFINE_TEST0 -DDEFINE_TEST2
DTEST1NYN = -DCALL_TEST1 -DDEFINE_TEST1
DTEST1YYN = -DCALL_TEST1 -DDEFINE_TEST1 -DDEFINE_TEST0
DTEST1YYY = -DCALL_TEST1 -DDEFINE_TEST1 -DDEFINE_TEST0 -DDEFINE_TEST2
DTEST1NYY = -DCALL_TEST1 -DDEFINE_TEST1 -DDEFINE_TEST2
DTEST2NNY = -DCALL_TEST2 -DDEFINE_TEST2
DTEST2YNY = -DCALL_TEST2 -DDEFINE_TEST2 -DDEFINE_TEST1
DTEST2YYY = -DCALL_TEST2 -DDEFINE_TEST2 -DDEFINE_TEST1 -DDEFINE_TEST2
DTEST2NYY = -DCALL_TEST2 -DDEFINE_TEST2 -DDEFINE_TEST1
DTEST01YYN = -DCALL_TEST0 -DCALL_TEST1 -DDEFINE_TEST0 -DDEFINE_TEST1
DTEST02YNY = -DCALL_TEST0 -DCALL_TEST2 -DDEFINE_TEST0 -DDEFINE_TEST2
DTEST12NYY = -DCALL_TEST1 -DCALL_TEST1 -DDEFINE_TEST1 -DDEFINE_TEST2
DTEST01YYY = -DCALL_TEST0 -DCALL_TEST1 -DDEFINE_TEST0 -DDEFINE_TEST1 -DDEFINE_TEST2
DTEST02YYY = -DCALL_TEST0 -DCALL_TEST2 -DDEFINE_TEST0 -DDEFINE_TEST1 -DDEFINE_TEST2
DTEST12YYY = -DCALL_TEST1 -DCALL_TEST1 -DDEFINE_TEST0 -DDEFINE_TEST1 -DDEFINE_TEST2
DTESTLIST = DTEST0YNN DTEST0YYN DTEST0YYY DTEST0YNY \
	    DTEST1NYN DTEST1YYN DTEST1YYY DTEST1NYY \
	    DTEST2NNY DTEST2YNY DTEST2YYY DTEST2NYY \
	    DTEST01YYN DTEST02YNY DTEST12NYY \
	    DTEST01YYY DTEST02YYY DTEST12YYY
DTESTALL = -DDEFINE_TEST0 -DDEFINE_TEST1 -DDEFINE_TEST2 -DCALL_TEST0 -DCALL_TEST1 -DCALL_TEST2

dtests1 = $($(1))
dtests = $(call dtests1,$(1))

all: CFLAGS+=$(DTESTALL)
all: $(TARGETS) $(LTO_TARGETS)

override CFLAGS += -fdiagnostics-color=auto \
		   -fdiagnostics-format=text \
		   -fdiagnostics-show-cwe \
		   -fmessage-length=0 \
		   -O2 -g3 \
		   -Wall \
		   -Wextra \
		   -Werror
CC=gcc
VALGRIND=valgrind --error-exitcode=1 --leak-check=full

%.o : %.c
	gcc $(CFLAGS) -c -o $@ $^

%.lto.o : %.c
	gcc -flto $(CFLAGS) -c -o $@ $^

$(TARGETS) :: % : %.o %.main.o
	gcc $(CFLAGS) -o $@ $^

$(LTO_TARGETS) :: %.lto : %.lto.o %.main.lto.o
	gcc -flto $(CFLAGS) -o $@ $^

clean :
	@rm -f *.o *.lto.o *.lto $(TARGETS)

.ONESHELL:
test :
	@for x in $(TARGETS) $(LTO_TARGETS) ; do
		echo "Building ${x} with all tests enabled and called"
		if make clean $${x} CFLAGS+="-fanalyzer $(DTESTALL)" ; then
			./$${x}
		elif make clean $${x} CFLAGS+="$(DTESTALL)" ; then
			$(VALGRIND) ./$${x}
		else
			echo "Could not build $${x}"
			exit 1
		fi
		$(foreach DTESTNAME,$(DTESTLIST),
			DTESTS="$(call dtests,$(DTESTNAME))" ;
			echo "Building $${x} with \"$${DTESTS}\""
			if $(MAKE) CFLAGS+="$${DTESTS} -fanalyzer" clean $${x} ; then
				./$${x}
			elif $(MAKE) CFLAGS+="$${DTESTS}" clean $${x} ; then
				$(VALGRIND) ./$${x}
			else
				echo "Could not build $${x} with $${DTESTS}"
				exit 1
			fi ;
		)
	done

.PHONY: all clean default test
# vim:ft=make
