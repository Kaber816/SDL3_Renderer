program: obj_renderer.c
	gcc obj_renderer.c -o obj_renderer `pkg-config --cflags --libs sdl3` -lm

clean:
	rm -f obj_renderer 

