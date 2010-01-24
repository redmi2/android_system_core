AR = ar
CC = gcc

SRC = rsa.c sha.c
OBJ = rsa.o sha.o

CFLAGS =  -I../include
CFLAGS += -c -fno-exceptions -Wno-multichar -m32 -fPIC 
CFLAGS += -fmessage-length=0 
CFLAGS += -W -Wall -Wno-unused -Winit-self -Wpointer-arith
CFLAGS += -O2 -g -fno-strict-aliasing
CFLAGS += -DSURF8K

ARFLAGS = crs

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

libmincrypt.a: $(OBJ)
	$(AR) $(ARFLAGS) $@ $^

.PHONY : clean
clean:
	rm -rf $(OBJ) libmincrypt.a
