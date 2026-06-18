CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic
LIBS=
SDIR=src
ODIR=obj
_OBJ = main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c
	[ -d $(ODIR) ] || mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

splicer: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -rf $(ODIR) splicer
