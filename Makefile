# Program Arguments
#
# Copyright (C) 2006
# Jeff Perry
# Center for Perceptual Systems
# University of Texas at Austin
#
# jsp Fri Aug 25 10:45:50 CDT 2006

INCLUDES=-I../utils
SRCS=\
	argv.cpp \
	minimal_example.cpp \
	touch_example.cpp \
	test_argv.cpp
OBJS=\
	argv.o
TARGETS=\
	test_argv \
	minimal_example \
	touch_example

include ../Makefile.include

check: all
	./test_argv
