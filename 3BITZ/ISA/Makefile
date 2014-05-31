CC =g++
CFLAGS =-Wall -pedantic -Wextra
PROJNAME =trace.cpp
RESULT =trace
LOGIN = xzaklo00
FILES = Makefile Readme manual.pdf trace.cpp

$(RESULT): $(PROJNAME)
	$(CC) $(CFLAGS) $(PROJNAME) -o $(RESULT)

clean:
	rm -f *~
	rm -f $(RESULT)

tar: clean
	tar -cf $(LOGIN).tar $(FILES)

rmtar:
	rm -f $(LOGIN).tar
