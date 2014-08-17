CC = cc
CFLAGS = -O0 -std=c99 -Wall
INCS = 
LIBS = -lm

all: mandelbrot

mandelbrot: mandelbrot.c
	$(CC) $(CFLAGS) $(INCS) $(LIBS) -o $@ $^

mandelbrot_db: mandelbrot_db.c
	$(CC) $(CFLAGS) $(INCS) $(LIBS) -o $@ $^

clean:
	rm -f mandelbrot
