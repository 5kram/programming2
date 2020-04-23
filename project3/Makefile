CC = gcc 
CCFLAGS = -Wall -g -std=c89
OBJ = project3.o objdb.o 

project3: $(OBJ)
	$(CC) $(OBJ) -o project3

project3.o: project3.c objdb.c objdb.h
	$(CC) $(CCFLAGS) -c $<

objdb.o: objdb.c objdb.h
	$(CC) $(CCFLAGS) -c $<

.PHONY: clean
clean:
	rm -f *.o project3
