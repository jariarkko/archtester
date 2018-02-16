
all:

install:
	cp archtester /sbin
	cp archtesterd /etc/init.d/
	update-rc.d archtesterd defaults
