CPPFLAGS=$(shell sdl2-config --cflags) $(EXTRA_CPPFLAGS) -std=c++11
LDLIBS=$(shell sdl2-config --libs) -lGLEW $(EXTRA_LDLIBS)
EXTRA_LDLIBS?=-lGL -lGLU -lglut
all: finalproject
clean:
	rm -f *.o finalproject
.PHONY: all clean

