window: glad.o stb_image.o texture.o shader.o
	@echo Compiling main and linking object files
	@g++ src/main.cpp build/**.o -o window -Iinclude -Llib -lglfw3 -lgdi32
	@./window

glad.o: src/glad.c
	@echo Compiling glad
	@gcc -c src/glad.c -o build/glad.o -Iinclude/ -Llib/ -lgdi32

stb_image.o: src/stb_image.c
	@echo Compiling stb_image
	@gcc -c src/stb_image.c -o build/stb_image.o -Iinclude/

texture.o: src/texture.cpp src/texture.hpp
	@echo Compiling texture.cpp
	@g++ -c src/texture.cpp -o build/texture.o -Iinclude/

shader.o: src/shader.cpp src/shader.hpp
	@echo Compiling shader.cpp
	@g++ -c src/shader.cpp -o build/shader.o -Iinclude/
