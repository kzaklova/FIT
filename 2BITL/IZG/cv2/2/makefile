################################################################################
# IZG labs, framework compilation
################################################################################

################################################################################
# promenne prekladu

ifeq ($(OS), Windows_NT)
  LIBS=-lopengl32 -lglu32 -lglut32 #-mwindows
else
  LIBS=-lGL -lGLU -lglut -lXmu -lX11 -lXi -L/usr/X11R6/lib
endif

CPP  = g++
CC   = gcc

INCS =
CXXINCS =
CXXFLAGS = -Wall $(CXXINCS) -DGLUT_NO_LIB_PRAGMA -DGLUT_NO_WARNING_DISABLE
CFLAGS = $(INCS)  -fmessage-length=0

################################################################################
# obecny cil kompilace
all: main.exe

################################################################################
# linkovani vysledneho souboru
main.exe:	main.o student.o
	$(CPP) main.o student.o -o main.exe $(LIBS)
#	strip main.exe

################################################################################
# kompilace hlavniho souboru
main.o: main.cpp
	$(CPP) -c main.cpp -o main.o $(CXXFLAGS)

################################################################################
# kompilace studentskeho souboru
student.o: student.cpp
	$(CPP) -c student.cpp -o student.o $(CXXFLAGS)

################################################################################
# cil vycisteni souboru prekladu
clean: clear
clear:
	rm -f *.o *.exe

################################################################################
################################################################################
