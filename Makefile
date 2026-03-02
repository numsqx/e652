# E652.
#

CWARNS= \
  -Wshadow \
  -Wundef \
  -Wwrite-strings \
  -Wdisabled-optimization \
  -Wmissing-declarations \
  # -pedantic

MYCFLAGS= $(CWARNS) -std=c99
MYLDFLAGS=

CC= gcc
RM= rm -rf
MKDIR= mkdir -p

CFLAGS= -Wall -Wextra $(MYCFLAGS) -MMD -MP
LDFLAGS= $(MYLDFLAGS)

## build file names

ALL_C= main.c
ALL_O= $(ALL_C:.c=.o)
ALL_D= $(ALL_C:.c=.d)
ALL_T= e652

.PHONY: all debug release ci test clean help echo

all: $(ALL_T)
debug release ci: all

ci:
	$(MAKE) test

debug:    CFLAGS+= -Og -g
release:  CFLAGS+= -O2 -DNDEBUG -march=native
ci:       CFLAGS+= -Werror

test: all
	@echo 'No tests yet.'

clean:
	$(RM) $(ALL_O) $(ALL_D) $(ALL_T)

help:
	@echo 'E652 Makefile'
	@echo '  help        Show this help'
	@echo '  clean       Remove generated files'
	@echo '  echo        Print make vars'
	@echo '  test        Run tests'
	@echo 'Build targets'
	@echo '  all         Default (plain) build'
	@echo '  debug       For debugging'
	@echo '  release     Optimized build'
	@echo '  ci          For CI checking'

echo:
	@echo 'CC = $(CC)'
	@echo 'RM = $(RM)'
	@echo 'MYCFLAGS = $(MYCFLAGS)'
	@echo 'MYLDFLAGS = $(MYLDFLAGS)'

$(ALL_T): $(ALL_O)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

-include $(ALL_D)
