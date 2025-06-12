#include "renderer.hpp"

#include <iostream>
#define LOG(...) printf(__VA_ARGS__)

int main() {
	auto window = GLFWwindow_create(800, 600, "gl study!");
	if (!window) {
		LOG("Failed to create window\n");
		return -1;
	}
	auto r = Renderer(window);
	Seconds currTime = glfwGetTime();
	Seconds delta = 0.f;
	while (!glfwWindowShouldClose(window)) {
		delta = glfwGetTime() - currTime;
		currTime = glfwGetTime();
		r.process(delta, glm::vec4(.3f, .3f, .3f, 1.f));
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
