################################################################################
# IZG labs, framework compilation
################################################################################

################################################################################
# promenne prekladu

ifeq ($(OS), Windows_NT)
  LIBS = -lopengl32 -lglu32 -lglut32 #-mwindows
  NAME = main.exe
else
  LIBS=-lGL -lGLU -lglut -lXmu -lX11 -lXi -L/usr/X11R6/lib
  NAME = main
endif

CPP  = g++
CC   = gcc

INCS =
CXXINCS =
CXXFLAGS = -Wall $(CXXINCS) -DGLUT_NO_LIB_PRAGMA -DGLUT_NO_WARNING_DISABLE
CFLAGS = $(INCS)  -fmessage-length=0

################################################################################
# obecny cil kompilace
all: $(NAME)

################################################################################
# linkovani vysledneho souboru
$(NAME):	main.o student.o
	$(CPP) main.o student.o -o $(NAME) $(LIBS)
#	strip $(NAME)

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
