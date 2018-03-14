
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

CFLAGS=		-g

CC=		cc
LD=		cc

all:	$(PROGRAMS)

install:	$(PROGRAMS)
	apt-get install bc
	cp $(PROGRAMS_BIN) /sbin
	cp $(PROGRAMS_INIT) /etc/init.d/
	update-rc.d archtester defaults

wc:
	wc -l $(SOURCES)

clean:
	-rm *~
