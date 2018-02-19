
PROGRAMS=	archtester \
		archtesterd \
		archtesterd_tlds \
		archtesterd_hops

SOURCES=	archtester \
		archtesterd \
		archtesterd_tlds \
		archtesterd_hops.c \
		Makefile

CFLAGS=		-g

CC=		cc

all:	$(PROGRAMS)

archtesterd_hops:	$(SOURCES)
	$(CC) $(CFLAGS) -c archtesterd_hops.c -o archtesterd_hops

install:	$(PROGRAMS)
	apt-get install bc
	cp archtesterd /sbin
	cp archtester /etc/init.d/
	update-rc.d archtester defaults

wc:
	wc -l $(SOURCES)
