CFLAGS = -std=c90 -pedantic -Wall -g
CFILES = worley_project3.c
TARGET = Worley-project3


project:
	gcc -o $(TARGET) $(CFLAGS) $(CFILES)

test:	
	./$(TARGET) y86-instructions.txt > log
