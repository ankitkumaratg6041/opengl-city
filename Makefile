# Compiler and flags
CC = g++
CFLAGS = -Iinclude -std=c++11 -g -Wall
LIBS = -lglut -lGLEW -lGL -lGLU

# Targets
TARGETS = main testCar
MAIN_OBJECTS = main.o Cuboid.o InitShader.o
TESTCAR_OBJECTS = testCar.o Cuboid.o InitShader.o Car.o

# Default rule
all: $(TARGETS)

# Link the main executable
main: $(MAIN_OBJECTS)
	$(CC) $(CFLAGS) -o main $(MAIN_OBJECTS) $(LIBS)

# Link the testCar executable
testCar: $(TESTCAR_OBJECTS)
	$(CC) $(CFLAGS) -o testCar $(TESTCAR_OBJECTS) $(LIBS)

# Compile main file
main.o: main.cc Cuboid.h
	$(CC) $(CFLAGS) -c main.cc

# Compile testCar file
testCar.o: testCar.cc Car.h Cuboid.h
	$(CC) $(CFLAGS) -c testCar.cc

# Compile Car class
Car.o: Car.cc Car.h Cuboid.h
	$(CC) $(CFLAGS) -c Car.cc

# Compile Cuboid class
Cuboid.o: Cuboid.cc Cuboid.h
	$(CC) $(CFLAGS) -c Cuboid.cc

# Compile InitShader utility
InitShader.o: common/InitShader.cc
	$(CC) $(CFLAGS) -c common/InitShader.cc -o InitShader.o

# Run memory checks for main
check-main: main
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./main

# Run memory checks for testCar
check-testCar: testCar
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./testCar

# Clean rule to remove all generated files
clean:
	rm -f $(TARGETS) *.o *~
