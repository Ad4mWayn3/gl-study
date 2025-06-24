#include "renderer.hpp"

#include <array>
#include <iostream>

enum CubeVertices {
	bottomFrontRight,
	bottomFrontLeft,
	topFrontRight,
	topFrontLeft,
	bottomBackRight,
	bottomBackLeft,
	topBackRight,
	topBackLeft,
};

std::array<float, 24> cube(glm::vec3 origin, float sideLength) {
	float x = origin.x, y = origin.y, z = origin.z, l = sideLength;
	return {
		x,y,z,
		x+l,y,z,
		x,y,z+l,
		x+l,y,z+l,
		
		x,y+l,z,
		x+l,y+l,z,
		x,y+l,z+l,
		x+l,y+l,z+l,
	};
};

struct VertexArray {
	std::vector<float> data;
	size_t attribCount;
};

int main() {
	auto window = GLFWwindow_create(800, 600, "gl study!");
	if (!window) {
		LOG("Failed to create window\n");
		return -1;
	}
	auto r = Renderer(window);
	auto& cUp = r.cameraU.data.up;
	assert(typeid(cUp.x) == typeid(float));
	Seconds currTime = glfwGetTime();
	Seconds delta = 0.f;
	while (!glfwWindowShouldClose(window)) {
		delta = glfwGetTime() - currTime;
		currTime = glfwGetTime();
		r.process(delta, glm::vec4(.3f, .3f, .3f, 1.f));
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	return 0;
}
