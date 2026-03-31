program: renderer.c
	gcc renderer.c -o renderer `pkg-config --cflags --libs sdl3` -lm

clean:
	rm -f renderer 
