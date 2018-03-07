
PROGRAMS=	archtester \
		archtesterd \
		archtesterd_tlds \
		archtesterd_tlsver \
		archtesterd_hops

SOURCES=	archtester \
		archtesterd \
		archtesterd_tlds \
		archtesterd_tlsver.c \
		archtesterd_hops \
		Makefile

OBJECTS=	archtesterd_tlsver.o

CFLAGS=		-g

CC=		cc
LD=		cc

all:	$(PROGRAMS)

archtesterd_hops:	$(SOURCES) $(OBJECTS)
	$(LD) $(CFLAGS) archtesterd_tlsver.o -o archtesterd_tlsver

archtesterd_tlsver.o:	$(SOURCES)
	$(CC) $(CFLAGS) -c archtesterd_tlsver.c

install:	$(PROGRAMS)
	apt-get install bc
	cp archtesterd archtesterd_hops archtesterd_tlds /sbin
	cp archtester /etc/init.d/
	update-rc.d archtester defaults

wc:
	wc -l $(SOURCES)

clean:
	-rm *~
