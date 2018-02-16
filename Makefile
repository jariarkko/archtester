
all:

install:
	cp archtesterd /sbin
	cp archtester /etc/init.d/
	update-rc.d archtester defaults
