################################################################################
# IZG lab #07, framework compilation
################################################################################

################################################################################
# promenne prekladu

ifeq ($(OS), Windows_NT)
  LIBS=-lopengl32 -lglu32 -lglut32 #-mwindows
  REMOVE = del
  BIN = main.exe
else
  LIBS=-lGL -lGLU -lglut -lXmu -lX11 -lXi -L/usr/X11R6/lib
  REMOVE = rm -f
  BIN = main
endif

CPP  = g++
CC   = gcc

INCS =
CXXINCS =
CXXFLAGS = -g3 -Wall $(CXXINCS) -DGLUT_NO_LIB_PRAGMA -DGLUT_NO_WARNING_DISABLE
CFLAGS = $(INCS)  -fmessage-length=0

################################################################################
# obecny cil kompilace
all: $(BIN)

################################################################################
# linkovani vysledneho souboru
$(BIN):	main.o student.o model.o
	$(CPP) main.o student.o model.o -o $(BIN) $(LIBS)
#	strip $(BIN)

################################################################################
# kompilace hlavniho souboru
main.o: main.cpp main.h
	$(CPP) -c main.cpp -o main.o $(CXXFLAGS)

################################################################################
# kompilace studentskeho souboru
student.o: student.cpp main.h
	$(CPP) -c student.cpp -o student.o $(CXXFLAGS)

################################################################################
# kompilace dilcich souboru
model.o: model.cpp model.h
	$(CPP) -c model.cpp -o model.o $(CXXFLAGS)

################################################################################
# cil vycisteni souboru prekladu
clean: clear
clear:
	$(REMOVE) *.o $(BIN)

################################################################################
################################################################################
