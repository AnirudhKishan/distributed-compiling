CC	= gcc
LINKER	= gcc
CFLAGS_F = -funsigned-char
ifdef DEBUG
 CFLAGS_F += -g -ggdb -DDEBUG
else
 CFLAGS_F += -O3
endif
CFLAGS_W = -W -Wall -Wpointer-arith -Wno-system-headers -Wshadow -Wbad-function-cast
CFLAGS	= $(CFLAGS_F) -ansi -Wall $(CFLAGS_W)
OBJ	= bf.o tty.o errors.o main.o
BINARY	= bf
CLEAN	= *~ *.bak *.o $(BINARY)
ifndef DEBUG
 LDFLAGS = -s
endif

.SUFFIXES: .c .o

all: $(BINARY)

$(BINARY): $(OBJ)
	$(LINKER) $(LDFLAGS) $(LIBS) $(OBJ) -o "$(BINARY)"

%o: %c Makefile
	$(CC) $(CFLAGS) -c -o $@	$<

bf.o: bf.c bf.h errors.h
main.o: main.c bf.h errors.h tty.h
errors.o: errors.c errors.h
tty.o: tty.c tty.h bf.h

clean:
	$(RM) $(CLEAN)

install: $(BINARY) $(BINARY).1
	install -D $(BINARY) $(DESTDIR)/usr/bin/$(BINARY)
	install -D $(BINARY).1 $(DESTDIR)/usr/share/man/man1/$(BINARY).1
	gzip -f9 $(DESTDIR)/usr/share/man/man1/$(BINARY).1
	# Examples are not installed!
