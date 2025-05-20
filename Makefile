objects = $(wildcard *.o)

window: build/$(objects) build/glm.hpp.gch
	@echo Compiling main and linking object files
	@g++ src/main.cpp build/**.o -o window -Iinclude -Ibuild -Llib -lglfw3 -lgdi32
	@./window

clean:
	@echo This target is not yet done.

build/texture.o: src/texture.cpp src/texture.hpp
	@echo Compiling texture.cpp
	g++ -c src/texture.cpp -o build/texture.o -Iinclude/

build/shader.o: src/shader.cpp src/shader.hpp
	@echo Compiling shader.cpp
	g++ -c src/shader.cpp -o build/shader.o -Iinclude/

build/glad.o: src/glad.c
	@echo Compiling glad
	gcc -c src/glad.c -o build/glad.o -Iinclude/ -Llib/ -lgdi32

build/glm.hpp.gch: src/glm.hpp
	@echo Precompiling glm header files
	g++ src/glm.hpp -o build/glm.hpp.gch -Iinclude/

build/stb_image.o: src/stb_image.c
	@echo Compiling stb_image
	gcc -c src/stb_image.c -o build/stb_image.o -Iinclude/
