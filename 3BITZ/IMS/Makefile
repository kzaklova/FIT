CLAGS=-g -O2 
PFLAGS=-lsimlib -lm
 
CC = g++
AUTHOR = xzaklo00_xbrych01
FILES = VerejneZak.cpp Makefile dokumentace.pdf

all: VerejneZak

VerejneZak: VerejneZak.cpp
	$(CC) $(CFLAGS) -o $@ VerejneZak.cpp $(PFLAGS)

rebuild: clean all

#argumenty programu
#argv1 pocet zamestnancu
#argv2 pocet soubezne zkoumanych podnetu pro jednoho zamestnance
#argv3 pocet soubezne zpracovavanych 2 instancnich rizeni 1 zamestnancem
#argv4 pocet rizeni zpracovavanych najednou jednim zamestnancem
#argv5 doba behu simulace(roky), pouze celociselne
#argv6 nazev vystupniho souboru(bude dodana pripona .out)
run:
	./VerejneZak d d d d d experiment1		#testujeme defaultni hodnoty, chovani systemu
	
	./VerejneZak 40 3 d 3 d experiment2		#testujeme mirnou optimalizaci poctu pracovniku a maximalni pocet dokumentu pro jednoho pracovnika(1 instance rizeni)
	
	./VerejneZak d d 20 d d experiment3		#testujeme pocet zpracovanych dokumentu 1 clena komise 2 instancniho rizeni
	
	./VerejneZak 40 3 20 3 d experiment4		#pokousime se o maximalni optimalizaci na zaklade predchozich experimentu
	
	./VerejneZak d 10 1 1 d experiment5	
	
clean:
	rm -f VerejneZak *.tar.gz *.zip *.out

tarball:
	tar -zcf $(AUTHOR).tar.gz $(FILES)

tar:
	tar -cf $(AUTHOR).tar $(FILES)

zip:
	zip 07_$(AUTHOR).zip $(FILES)
