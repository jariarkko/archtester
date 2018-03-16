
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
	apt-get install -y bc wget
	(cd ..; git clone https://github.com/jariarkko/hopping.git; cd hopping; sudo make all install)
	(cd ..; git clone https://github.com/jariarkko/tlsver.git; cd tlsver; sudo make all install)
	cp $(PROGRAMS_BIN) /sbin
	cp $(PROGRAMS_INIT) /etc/init.d/
	update-rc.d archtester defaults

wc:
	wc -l $(SOURCES)

clean:
	-rm *~
