LIBS= `pkg-config --libs opencv`
CFLAGS= `pkg-config --cflags opencv`

objects= main.o

tcp: $(objects) 
	gcc main.c -o tcp $(LIBS)$(CFLAGS)
	sync

.PHONY: clean
clean:
	rm tcp $(objects)
