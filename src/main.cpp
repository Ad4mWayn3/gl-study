#include "renderer.hpp"

#include <iostream>

int main() {
	auto window = GLFWwindow_create(800, 600, "gl study!");
	if (!window) {
		LOG("Failed to create window\n");
		return -1;
	}
	auto r = Renderer(window);
	LOG("main: successfully created renderer\n");
	Seconds currTime = glfwGetTime();
	Seconds delta = 0.f;
	while (!glfwWindowShouldClose(window)) {
		delta = glfwGetTime() - currTime;
		currTime = glfwGetTime();
		LOG("main loop: everything alright before process()\n");
		r.process(delta, glm::vec4(.3f, .3f, .3f, 1.f));
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
