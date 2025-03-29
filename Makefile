window: glad.o
	@g++ src/main.cpp build/glad.o -o window -Iinclude -Llib -lglfw3 -lgdi32
	@./window

glad.o: src/glad.c
	@gcc -c src/glad.c -o build/glad.o -Iinclude/ -Llib/ -lgdi32