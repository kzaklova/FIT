# prekladac C
CC=gcc
# CURL
CURL=-I../curl/include/ -L../curl/lib/ # funkcni verze pro projekt na IVS serveru - nevytvorila se slozka .libs
#CURL=-I../curl/include/ -L../curl/lib/.libs/ # podle zadani projektu
LIBS=-lcurl
# parametry prekladace
CFLAGS=-std=c99 -Wall -pedantic -W -Wextra $(CURL)
# nazev souboru
LOGIN=xzaklo00
# seznam objektovych souboru
FILES= main.o down.o parity.o
# soubory k odevzdani
PACK=.git/ *.c *.h mockup.pdf protokol.pdf makefile tests.sh Doxyfile
# testy
TESTS=tests.sh
# konfigurace doxygenu
DOX=Doxyfile

# generovani objektovych souboru
%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $< $(LIBS)


default: $(FILES)
	$(CC) $(CFLAGS) -o $(LOGIN) $^ $(LIBS)

# priznaky pro debugging
debug:CFLAGS+= -g3
debug:default

# priznaky pro profiling
profiling:CFLAGS+= -pg
profiling:default

# zabali soubory k odevzdani
pack:clean
	zip -r $(LOGIN).zip $(PACK)

# vycisti nepotrebne soubory
clean:
	rm -f *~ *.o xzaklo00 xzaklo00.zip odd1 odd2 odd3 even1 even2 even3 *.html

# spusti testy
test:
	chmod +x $(TESTS)
	./$(TESTS)

#vygeneruje doxygen
doc:
	doxygen $(DOX)
