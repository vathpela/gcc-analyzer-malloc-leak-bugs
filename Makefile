# SPDX-License-Identifier: GPLv2-or-later
#
# Makefile
# Copyright Peter Jones <pjones@redhat.com>
#
default: all

TARGETS=analyzer-malloc-leak
LTO_TARGETS=$(foreach x,$(TARGETS),${x}.lto)
MAXTEST=1

DTEST0YN = -DCALL_TEST0 -DDEFINE_TEST0
DTEST0YY = -DCALL_TEST0 -DDEFINE_TEST0 -DDEFINE_TEST1
DTEST1NY = -DCALL_TEST1 -DDEFINE_TEST1
DTESTLIST = DTEST0YN DTEST0YY DTEST1NY
DTESTALL = -DDEFINE_TEST0 -DDEFINE_TEST1 -DCALL_TEST0 -DCALL_TEST1

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
