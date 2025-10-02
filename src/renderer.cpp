#include "renderer.hpp"

void frameBufferResize(GLFWwindow* window, int width, int height) {
	glViewport(0,0, width, height);
}

GLFWwindow* GLFWwindow_create(int width, int height, const char* title) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, title,
		NULL, NULL);
	
	if (!window) {
		std::cout << "Failed to create GLFW window\n";
		glfwTerminate();
		return nullptr;
	}
	glfwSetFramebufferSizeCallback(window, frameBufferResize);
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { 
		std::cout << "Failed to initialize GLAD\n";
		return nullptr;
	}

	glViewport(0,0, width, height);
	return window;
}

glm::vec2 curPos(GLFWwindow* window) {
	double x,y;
	glfwGetCursorPos(window, &x, &y);
	return {(float)x, (float)y};
}

glm::vec2 winRes(GLFWwindow* window) {
	int x,y;
	glfwGetWindowSize(window, &x, &y);
	return {(float)x, (float)y};
}

Mesh Mesh::create(std::vector<float>&& vertices, std::vector<int>&& indices) {
	Mesh out;
	out.vertices = vertices;
	out.indices = indices;
	out.stride = 5 * sizeof(float);
	
	GLuint VBO=0, EBO=0, VAO=0;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, out.vertices.size() * sizeof(out.vertices[0]),
		out.vertices.data(), GL_DYNAMIC_DRAW);
//	assert(sizeof(out.vertices) == 
//		out.vertices.size() * sizeof(out.vertices[0]));

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, out.indices.size() * sizeof(out.indices[0]),
		out.indices.data(), GL_DYNAMIC_DRAW);
//	assert(sizeof(out.indices) ==
//		out.indices.size() * sizeof(out.indices[0]));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, out.stride,
		(GLvoid*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, out.stride,
		(GLvoid*)(3 * sizeof(float)));
	out.VBO = VBO;
	out.VAO = VAO;
	out.EBO = EBO;
	return out;
}

Renderer::Renderer(GLFWwindow* window) : program{0} {
	glEnable(GL_DEPTH_TEST);
	int w=0, h=0;
	glfwGetWindowSize(window, &w, &h);
	auto resolution = glm::vec2((float)w, (float)h);
	this->window = window;
	mousePos = curPos(window);

	auto vertices = std::vector{
		// front vertices
		-.2f, -.2f, 0.f,	0.0f, 0.0f, // bottom left
		.8f,  -.2f, 0.f,	1.0f, 0.0f, // bottom right
		-.2f,  .8f, 0.f,	0.0f, 1.0f, // top left
		.8f,   .8f, 0.f,	1.0f, 1.0f, // top right

		// back vertices
		-.2f, -.2f, 1.f,	1.0f, 1.0f, // bottom left
		.8f,  -.2f, 1.f,	0.0f, 1.0f, // bottom right
		-.2f,  .8f, 1.f,	1.0f, 0.0f, // top left
		.8f,   .8f, 1.f,	0.0f, 0.0f, // top right
	};
	auto indices = std::vector{
		//front
		0, 1, 2,
		1, 2, 3,
		//back
		4, 5, 6,
		5, 6, 7,
		//bottom
		0, 1, 4,
		1, 4, 5,
		//top
		2, 7, 3,
		7, 2, 6,
		//left
		0, 2, 4,
		2, 4, 6,
		//right
		1, 3, 5,
		3, 5, 7,
	};
	mesh = Mesh::create(std::move(vertices), std::move(indices));

	program = (ShaderProgram&&)ShaderProgram::buildPath("src/vertex.glsl",
		"src/fragment.glsl");
	glUseProgram(program.obj);
	LOG("Renderer::Renderer(): glError %s\n", getErrorName(glGetError()));
	
	{ // texture init
		stbi_set_flip_vertically_on_load(true);
		Texture::setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
		Texture::setParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
		Texture::setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		Texture::setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		trollcake = Uniform<Texture>::create(
			(Texture&&)Texture::generate(GL_TEXTURE0)
				.bind()
				.loadFromPath("resources/trollcake.jpg")
				.unbind(),
			program.obj, "tex");
		derpina = Uniform<Texture>::create(
			(Texture&&)Texture::generate(GL_TEXTURE1)
				.bind()
				.loadFromPath("resources/derpina.jpg")
				.unbind(),
			program.obj, "tex2");
	}
	
	glUniform1i(glGetUniformLocation(program.obj, "tex"), 0);
	glUniform1i(glGetUniformLocation(program.obj, "tex2"), 1);
	struct { GLint id; float value; } redValue
		{ glGetUniformLocation(program.obj, "redValue"), 0 };

	float mixValue = 0.5f;
	float delta = 0.f;
	float currTime = 0.f;

	auto zigzag = [](float x) -> float {
		return 2. * glm::abs(x/2. - glm::floor(x/2. + 1./2.));
	};

	cameraU = Uniform<Camera>::create(
		Camera::fromTarget(glm::vec3(0.f, 0.f, 3.f),
			glm::vec3(0.f, 0.f, 0.f)),
		program.obj, "camera");
	//GLuint cameraLoc = program.getUniformId("camera");
	
#pragma region matrices
	const auto id4x4 = glm::mat4(1.);
	
	GLuint modelLoc = program.getUniformId("model"),
		viewLoc = program.getUniformId("view"),
		projectionLoc = program.getUniformId("projection");

	auto model = glm::rotate(id4x4, currTime * glm::radians(-55.0f),
		glm::vec3(0.5f,1.0f, 0.f));
	auto view = glm::lookAt(cameraU.data.pos,
		cameraU.data.getTarget(),
		glm::vec3(0.f, 1.f, 0.f));
	auto projection = glm::perspective(glm::radians(45.0f),
		(float)resolution.x / resolution.y, 0.1f, 100.0f);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	#define u(x) Uniform<glm::mat4>::assign(std::move(x), x##Loc)
	this->model = u(model);
	this->view = u(view);
	this->projection = u(projection);
	#undef u
#pragma endregion
}

glm::quat rotor(glm::vec3 axis, double theta) {
	LOG("rotor: axis.length() = %f\n", glm::length(axis));
	assert(glm::length(axis) - 1.f < 2e-3f);
	glm::quat out;
	out.w = cos(theta/2.f);
	float s = sin(theta/2.f);
	out.x = axis.x * s;
	out.y = axis.y * s;
	out.z = axis.z * s;
	return out;
}

void Renderer::processInput(Seconds delta) {
	float rSpeed = glm::radians(42.f * delta);
	float mSpeed = 1.f * delta;
	auto roll = glm::cross(-cameraU.data.up, cameraU.data.right),
		yaw = glm::cross(-cameraU.data.dir, cameraU.data.right),
		pitch = glm::normalize(glm::cross(cameraU.data.pos, cameraU.data.up));

	bool click = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS;
	
	if (mouseDelta.x != 0.f && click)
		cameraU.data.rotate(rotor(yaw, mouseDelta.x));
	if (mouseDelta.y != 0.f && click)
		cameraU.data.rotate(rotor(pitch, mouseDelta.y));
	
	#define kpress(k) (glfwGetKey(window, k) == GLFW_PRESS)
	if kpress(GLFW_KEY_R) {
		cameraU.data.rotate(rotor(roll, rSpeed));
	}
	if kpress(GLFW_KEY_W) {
		cameraU.data.rotate(rotor(roll, -rSpeed));
	}
	if kpress(GLFW_KEY_F) {
		cameraU.data.rotate(rotor(yaw, rSpeed));
	}
	if kpress(GLFW_KEY_S) {
		cameraU.data.rotate(rotor(yaw, -rSpeed));
	}
	if kpress(GLFW_KEY_E) {
		cameraU.data.pos += cameraU.data.dir * mSpeed;
		LOG("Renderer::processInput: camera.pos = {%f, %f, %f}\n",
			cameraU.data.pos.x, cameraU.data.pos.y, cameraU.data.pos.z);
	}
	if kpress(GLFW_KEY_D) {
		cameraU.data.pos -= cameraU.data.dir * mSpeed;
		LOG("Renderer::processInput: camera.pos = {%f, %f, %f}\n",
			cameraU.data.pos.x, cameraU.data.pos.y, cameraU.data.pos.z);
	}
	#undef kpress
	assert(glGetError() == GL_NO_ERROR);
}

void Renderer::process(Seconds delta, glm::vec4 clearColor) {
	assert(glGetError() == GL_NO_ERROR);
	auto winR = winRes(window);
	mouseDelta = curPos(window) - mousePos;
	mouseDelta = mouseDelta / winR * 3.f;
	mousePos = curPos(window);

	processInput(delta);
	assert(mesh.vertices[0] == -.2f);
	assert(mesh.VAO != 0);
	const static auto id4x4 = glm::mat4(1.);
	Seconds currTime = glfwGetTime();
	model.data = 
		// glm::rotate(id4x4, currTime * glm::radians(-55.f),
		// glm::vec3(.5f, 1.f, 0.f));
		id4x4;
	glUniformMatrix4fv(model.id, 1, GL_FALSE, glm::value_ptr(model.data));

	auto& camera = cameraU.data;
	view.data = glm::lookAt(camera.pos,
		camera.getTarget(),
		camera.up);
	glUniformMatrix4fv(view.id, 1, GL_FALSE, glm::value_ptr(view.data));

/*		auto vertexBuffer = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	vertexBuffer[25] = glm::sin(currTime * 2.)/2. + 1./2.;
	vertexBuffer[8] = zigzag(currTime);
	glUnmapBuffer(GL_ARRAY_BUFFER);
*/

	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// bind if another object was drawn previously;
	trollcake.data.bind();
	derpina.data.bind();
//		glBindTexture(GL_TEXTURE_2D, id);
//		glBindVertexArray(VAO);
//		glBindBuffer(GL_ARRAY_BUFFER, VBO);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, nullptr);
}
