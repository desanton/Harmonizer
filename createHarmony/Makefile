CC     = g++
cC     = $(CC) -c
CFLAG  = -g 
LFLAG  = -g
BIN    = harmonizer
TRASH  = *~ *.o *.sha256  a.out $(BIN)

all: $(BIN)

$(BIN): harmonizer.o
	$(CC) harmonizer.o $(LFLAG) -o $@

harmonizer.o: harmonizer.cpp
	$(cC) $(CFLAG) $^ -o $@

clean:
	rm -f $(TRASH)
	rm -rf *.o.dSYM
