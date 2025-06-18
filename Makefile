objects = build/texture.o build/shader.o build/glad.o build/stb_image.o \
	build/renderer.o build/glm.hpp.gch

window: $(objects)
	@echo Compiling main and linking object files
	g++ src/main.cpp build/**.o -o window -Iinclude -Ibuild -Llib -lglfw3 -lgdi32
	@./window

clear:
	@echo Cleaning build...
	@rm -f build/**o build/glm.hpp.gch window.exe
	@rmdir build

build/renderer.o: src/renderer.cpp src/renderer.hpp src/logging.h | build
	@echo Compiling renderer.cpp
	g++ -c src/renderer.cpp -o build/renderer.o -Iinclude/

build/texture.o: src/texture.cpp src/texture.hpp src/logging.h | build
	@echo Compiling texture.cpp
	g++ -c src/texture.cpp -o build/texture.o -Iinclude/

build/shader.o: src/shader.cpp src/shader.hpp src/logging.h | build
	@echo Compiling shader.cpp
	g++ -c src/shader.cpp -o build/shader.o -Iinclude/

build/stb_image.o: src/stb_image.c | build
	@echo Compiling stb_image
	gcc -c src/stb_image.c -o build/stb_image.o -Iinclude/

build/glm.hpp.gch: src/glm.hpp | build
	@echo Precompiling glm header files
	g++ src/glm.hpp -o build/glm.hpp.gch -Iinclude/

build/glad.o: src/glad.c | build
	@echo Compiling glad
	gcc -c src/glad.c -o build/glad.o -Iinclude/ -Llib/ -lgdi32

build:
	mkdir build
