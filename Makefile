
PROGRAMS=	archtester \
		archtesterd \
		archtesterd_tlds \
		archtesterd_hops

SOURCES=	archtester \
		archtesterd \
		archtesterd_tlds \
		archtesterd_hops.c \
		Makefile

OBJECTS=	archtesterd_hops.o

CFLAGS=		-g

CC=		cc
LD=		cc

all:	$(PROGRAMS)

archtesterd_hops:	$(SOURCES) $(OBJECTS)
	$(LC) $(CFLAGS) archtesterd_hops.o -o archtesterd_hops

archtesterd_hops.o:	$(SOURCES)
	$(CC) $(CFLAGS) -c archtesterd_hops.c

install:	$(PROGRAMS)
	apt-get install bc
	cp archtesterd /sbin
	cp archtester /etc/init.d/
	update-rc.d archtester defaults

wc:
	wc -l $(SOURCES)
