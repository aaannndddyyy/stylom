APP=stylom
VERSION=1.00

all:
	gcc -Wall -ansi -pedantic -o $(APP) src/*.c -Isrc -lm

debug:
	gcc -Wall -ansi -pedantic -g -o $(APP) src/*.c -Isrc -lm

source:
	rm -f *.o $(APP) $(APP)_tests *.png debian/*.log debian/*.substvars
	rm -rf debian/stylom
	tar -cvzf ../$(APP)_$(VERSION).orig.tar.gz --exclude=texts --exclude=.bzr --exclude=fingerprints ../$(APP)-$(VERSION)

tests:
	gcc -Wall -ansi -pedantic -g -o $(APP)_tests src/dictionary.c src/word.c src/fingerprint.c unittests/*.c -Isrc -Iunittests -lm

install:
	cp $(APP) /usr/bin
	cp man/$(APP).1.gz /usr/share/man/man1
	chmod 755 /usr/bin/$(APP)
	chmod 644 /usr/share/man/man1/$(APP).1.gz

clean:
	rm -f *.o $(APP) $(APP)_tests *.png debian/*.log debian/*.substvars
	rm -rf debian/stylom
