CC =g++
CFLAGS =-Wall -pedantic -Wextra
PROJNAME =webclient.cpp
RESULT =webclient
LOGIN = xzaklo00
FILES = Makefile webclient.cpp

$(RESULT): $(PROJNAME)
	$(CC) $(CFLAGS) $(PROJNAME) -o $(RESULT)

clean:
	rm -f *~
	rm -f $(RESULT)

tar: clean
	tar -cf $(LOGIN).tar $(FILES)
	gzip $(LOGIN).tar

rmtar:
	rm -f $(LOGIN).tar

