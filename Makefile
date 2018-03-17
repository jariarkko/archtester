
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
	(cd ..; if [ -d hopping ]; then (cd hopping; git pull); else git clone https://github.com/jariarkko/hopping.git; fi; cd hopping; sudo make all install)
	(cd ..; if [ -d tlsver ]; then (cd tlsver; git pull); else git clone https://github.com/jariarkko/tlsver.git; fi; cd tlsver; sudo make all install)
	cp $(PROGRAMS_BIN) /sbin
	cp $(PROGRAMS_INIT) /etc/init.d/
	update-rc.d archtester defaults

software.tgz:	$(SOURCES)
	-rm -f software.tgz
	tar czf software.tgz $(SOURCES)

copy_software:	software.tgz
	scp software.tgz root@cloud1.arkko.eu:/var/www/www.suckagetest.net/html/archtester/dev0001
	scp software.tgz root@cloud1.arkko.eu:/var/www/www.suckagetest.net/html/archtester/dev0002
	scp software.tgz root@cloud1.arkko.eu:/var/www/www.suckagetest.net/html/archtester/dev0003
	scp software.tgz root@cloud1.arkko.eu:/var/www/www.suckagetest.net/html/archtester/dev0004

wc:
	wc -l $(SOURCES)

clean:
	-rm *~
