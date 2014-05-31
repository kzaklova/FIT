################################################################################
# IZG lab #05, framework compilation
################################################################################

################################################################################
# build variables

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
# build target
all: $(BIN)

################################################################################
# linking target file
$(BIN):	main.o student.o transform.o model.o
	$(CPP) main.o student.o transform.o model.o -o $(BIN) $(LIBS)
#	strip $(BIN)

################################################################################
# building main file
main.o: main.cpp main.h
	$(CPP) -c main.cpp -o main.o $(CXXFLAGS)

################################################################################
# building students file
student.o: student.cpp main.h
	$(CPP) -c student.cpp -o student.o $(CXXFLAGS)

################################################################################
# building other files
transform.o: transform.cpp transform.h
	$(CPP) -c transform.cpp -o transform.o $(CXXFLAGS)
model.o: model.cpp model.h
	$(CPP) -c model.cpp -o model.o $(CXXFLAGS)

################################################################################
# clean built files
clean: clear
clear:
	$(REMOVE) *.o $(BIN)

################################################################################
################################################################################

