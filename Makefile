window: glad.o stb_image.o
	@g++ src/main.cpp build/glad.o -o window -Iinclude -Llib -lglfw3 -lgdi32
	@./window

glad.o: src/glad.c
	@gcc -c src/glad.c -o build/glad.o -Iinclude/ -Llib/ -lgdi32

stb_image.o: src/stb_image.c
	@gcc -c src/stb_image.c -o build/stb_image.o -Iinclude/
