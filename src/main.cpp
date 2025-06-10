#include "renderer.hpp"

#include <iostream>
#define LOG(...) printf(__VA_ARGS__)

int main() {
	auto window = GLFWwindow_create(1920, 1080, "gl study!");
	if (!window) {
		LOG("Failed to create window\n");
		return -1;
	}
	auto r = Renderer::init(window);
	Seconds currTime = glfwGetTime();
	Seconds delta = 0;
	while (!glfwWindowShouldClose(window)) {
		delta = glfwGetTime() - currTime;
		currTime = glfwGetTime();
		r.process(delta, glm::vec4(.6f, .6f, .6f, 1.f));
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
