
PROGRAMS_INIT=	archtester
PROGRAMS_BIN=	archtesterd \
		archtesterd_connectivity \
		archtesterd_tlds \
		archtesterd_tlsver \
		archtesterd_hops
PROGRAMS=	$(PROGRAMS_INIT) \
		$(PROGRAMS_BIN)
SOURCES=	archtester \
		archtesterd \
		archtesterd_connectivity \
		archtesterd_tlds \
		archtesterd_tlsver \
		archtesterd_hops \
		Makefile

OBJECTS=	archtesterd_tlsver.o

CFLAGS=		-g

CC=		cc
LD=		cc

all:	$(PROGRAMS)

archtesterd_tlsver:	$(SOURCES) $(OBJECTS)
	$(LD) $(CFLAGS) archtesterd_tlsver.o -o archtesterd_tlsver

archtesterd_tlsver.o:	$(SOURCES)
	$(CC) $(CFLAGS) -c archtesterd_tlsver.c

install:	$(PROGRAMS)
	apt-get install bc
	cp $(PROGRAMS_BIN) /sbin
	cp $(PROGRAMS_INIT) /etc/init.d/
	update-rc.d archtester defaults

wc:
	wc -l $(SOURCES)

clean:
	-rm *~
