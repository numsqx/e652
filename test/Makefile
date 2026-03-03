# E652 tests.
#

E652= ../e652

CA65= ca65
LD65= ld65
RM= rm -rf

CFG= generic.cfg

CAFLAGS=
LDFLAGS= -C $(CFG)

## test file names

ALL_S= $(wildcard *.asm)
ALL_O= $(ALL_S:.asm=.o)
ALL_B= $(ALL_S:.asm=.bin)
ALL_T= $(ALL_S:.asm=-test)

.PHONY: test bin clean help echo list

test: $(ALL_T) bin
bin: $(ALL_B)

%-test: %.bin | $(E652)
	@echo '=== TEST: $@'
	$(E652) $<

$(E652):
	$(MAKE) -C..

clean:
	$(RM) $(ALL_O) $(ALL_B)

help:
	@echo 'E652 tests'
	@echo '  help        Show this help'
	@echo '  clean       Remove generated files'
	@echo '  echo        Print make vars'
	@echo '  list        List all available tests'
	@echo 'By test'
	@echo '  test        Run all tests'
	@echo '  TEST-test   Run a test named "TEST"'
	@echo '  bin         Compile all tests'
	@echo '  TEST.bin    Compile a test named "TEST"'

echo:
	@echo 'CA65 = $(CA65)'
	@echo 'LD65 = $(LD65)'
	@echo 'RM = $(RM)'
	@echo 'CFG = $(CFG)'

list:
	@for t in $(ALL_T); do echo "$$t"; done

%.bin: %.o
	$(LD65) $(LDFLAGS) -o $@ $<

%.o: %.asm $(CFG)
	$(CA65) $(CAFLAGS) -o $@ $<
