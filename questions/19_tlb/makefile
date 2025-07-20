# Compiler and flags — change here if using gcc or different flags
CC = gcc
CFLAGS = -Wall -g -O0
EXT = c

# Output binary names
TARGETS = main main2 main3 test test1 test2 test3 test4 test5

.PHONY: all clean tests $(TARGETS)

# Build everything
all: $(TARGETS)

# Build all test programs with `make tests`
tests: test test1 test2 test3 test4 test5

# Individual test targets
test: test.$(EXT)
	$(CC) $(CFLAGS) $< -o $@

test1: test1.$(EXT)
	$(CC) $(CFLAGS) $< -o $@

test2: test2.$(EXT)
	$(CC) $(CFLAGS) $< -o $@

test3: test3.$(EXT)
	$(CC) $(CFLAGS) $< -o $@

test4: test4.$(EXT)
	$(CC) $(CFLAGS) $< -o $@

test5: test5.$(EXT)
	$(CC) $(CFLAGS) $< -o $@

# Main program targets
main: main.$(EXT)
	$(CC) $(CFLAGS) $< -o $@

main2: main2.$(EXT)
	$(CC) $(CFLAGS) $< -o $@

main3: main3.$(EXT)
	$(CC) $(CFLAGS) $< -o $@

# Clean rule
clean:
	rm -f *.o $(TARGETS)
	rm *.pdf *.png *.dat *.svg



# Run the TLB benchmark

# Only regenerate plots from existing data
plot-tlb:
	for fmt in png pdf svg; do \
	  case $$fmt in \
	    png) term="pngcairo size 800,600" ;; \
	    pdf) term="pdfcairo size 8,6 enhanced font 'Helvetica,10'" ;; \
	    svg) term="svg size 800,600" ;; \
	  esac; \
	  gnuplot -e "set terminal $$term; set output 'tlb_plot.$$fmt';" plot_tlb.gp; \
	done

