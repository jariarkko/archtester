
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
SOFTWAREPACKAGE=software.tgz

CFLAGS=		-g

CC=		cc
LD=		cc

all:	$(PROGRAMS) \
	$(SOFTWAREPACKAGE)

install:	$(PROGRAMS)
	apt-get install -y bc wget
	(cd ..; git clone https://github.com/jariarkko/hopping.git; cd hopping; sudo make all install)
	(cd ..; git clone https://github.com/jariarkko/tlsver.git; cd tlsver; sudo make all install)
	cp $(PROGRAMS_BIN) /sbin
	cp $(PROGRAMS_INIT) /etc/init.d/
	update-rc.d archtester defaults

software.tgz:	$(SOURCES)
	-rm -f software.tgz
	tar czf software.tgz $(SOURCES)

wc:
	wc -l $(SOURCES)

clean:
	-rm *~
