# Compiler and flags
CC = g++
CFLAGS = -Iinclude -std=c++11 -g
LIBS = -lglut -lGLEW -lGL -lGLU

# Targets
TARGET = main
OBJECTS = main.o Cuboid.o InitShader.o

# Default rule
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

# Compile main file
main.o: main.cc Cuboid.h
	$(CC) $(CFLAGS) -c main.cc

# Compile Cuboid class
Cuboid.o: Cuboid.cc Cuboid.h
	$(CC) $(CFLAGS) -c Cuboid.cc

# Compile InitShader utility
InitShader.o: common/InitShader.cc
	$(CC) $(CFLAGS) -c common/InitShader.cc -o InitShader.o

# Run memory checks with Valgrind
check: main
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose ./main


# Clean rule to remove all generated files
clean:
	rm -f $(TARGET) *.o *~
