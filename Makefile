APP=stylom
VERSION=1.00
ARCH_TYPE=`uname -m`

all:
	gcc -Wall -ansi -pedantic -o $(APP) src/*.c -Isrc -lm

debug:
	gcc -Wall -ansi -pedantic -g -o $(APP) src/*.c -Isrc -lm

source:
	tar -cvzf ../$(APP)_$(VERSION).orig.tar.gz --exclude=texts --exclude-vcs --exclude=fingerprints ../$(APP)-$(VERSION)

tests:
	gcc -Wall -ansi -pedantic -g -o $(APP)_tests src/dictionary.c src/word.c src/fingerprint.c unittests/*.c -Isrc -Iunittests -lm

install:
	cp $(APP) $(DESTDIR)/usr/bin
	cp man/$(APP).1.gz $(DESTDIR)/usr/share/man/man1
	chmod 755 $(DESTDIR)/usr/bin/$(APP)
	chmod 644 $(DESTDIR)/usr/share/man/man1/$(APP).1.gz

clean:
	rm -f *.o $(APP) $(APP)_tests *.png debian/*.log debian/*.substvars
	rm -rf debian/stylom
	rm -rf deb.* debian/$(APP) rpmpackage/$(ARCH_TYPE)
	rm -f ../$(APP)*.deb ../$(APP)*.changes ../$(APP)*.asc ../$(APP)*.dsc
	rm -f rpmpackage/*.src.rpm
