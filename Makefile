all: obj_renderer renderer

obj_renderer: obj_renderer.c
	gcc obj_renderer.c -o obj_renderer `pkg-config --cflags --libs sdl3` -lm

renderer: renderer.c
	gcc renderer.c -o renderer `pkg-config --cflags --libs sdl3` -lm

clean:
	rm -f obj_renderer renderer
