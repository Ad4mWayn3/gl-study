
#if 0
int r_main() {
#pragma region init
	struct { int w, h; } resolution {800, 600};
	auto window = GLFWwindow_create(resolution.w, resolution.h, "gl study!");
	if (!window) return -1;
	glEnable(GL_DEPTH_TEST);
	
	float square[] = {
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
	int squareIdxs[] = {
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
	GLsizei stride = 5 * sizeof(float);
#pragma region gpu vertex config
	GLuint VBO=0, EBO=0, VAO=0;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square), square,
		GL_DYNAMIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(squareIdxs), squareIdxs,
		GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
		(GLvoid*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride,
		(GLvoid*)(3 * sizeof(float)));
	//	unbind the array and attrib array, required if objects with different
	//  attributes will be defined
	//	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//	glBindVertexArray(0);
#pragma endregion
	
	ShaderProgram program = ShaderProgram::buildPath("src/vertex.glsl",
		"src/fragment.glsl");
	glUseProgram(program.obj);

	{ // texture parameter config
		stbi_set_flip_vertically_on_load(true);
		Texture::setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
		Texture::setParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
		Texture::setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		Texture::setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	auto trollcake = Texture::generate(GL_TEXTURE0)
		.bind()
		.loadFromPath("resources/trollcake.jpg")
		.unbind();
	auto derpina = Texture::generate(GL_TEXTURE1)
		.bind()
		.loadFromPath("resources/derpina.jpg")
		.unbind();
	
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

	auto camera = Camera::fromTarget(glm::vec3(0.f, 0.f, 3.f),
		glm::vec3(0.f, 0.f, 0.f));
	GLuint cameraLoc = program.getUniformId("camera");
	
#pragma region matrices
	const auto id4x4 = glm::mat4(1.);
	
	GLuint modelLoc = program.getUniformId("model"),
		viewLoc = program.getUniformId("view"),
		projectionLoc = program.getUniformId("projection");

	auto model = glm::rotate(id4x4, currTime * glm::radians(-55.0f),
		glm::vec3(0.5f,1.0f, 0.f));
	auto view = glm::lookAt(camera.pos,
		camera.getTarget(),
		glm::vec3(0.f, 1.f, 0.f));
		//glm::lookAt( camera.pos, camera.getTarget(), camera.up);
	auto projection = glm::perspective(glm::radians(45.0f),
		(float)resolution.w / resolution.h, 0.1f, 100.0f);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
#pragma endregion
#pragma endregion
	while (!glfwWindowShouldClose(window)) {
		glfwGetWindowSize(window, &resolution.h, &resolution.h);

		delta = glfwGetTime() - currTime;
		currTime += delta;

		model = glm::rotate(id4x4, currTime * glm::radians(-55.f),
			glm::vec3(.5f, 1.f, 0.f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		// view = glm::lookAt(camera.pos, camera.getTarget(), camera.up);
		// glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			
		glUniform1f(redValue.id, zigzag(mixValue));

/*		auto vertexBuffer = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		vertexBuffer[25] = glm::sin(currTime * 2.)/2. + 1./2.;
		vertexBuffer[8] = zigzag(currTime);
		glUnmapBuffer(GL_ARRAY_BUFFER);
*/
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			mixValue += 1.f * delta;
		} if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			mixValue -= 1.f * delta;
		} if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			camera.pos.x -= 1.f * delta;
		} if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
			camera.pos.x += 1.f * delta;
		}
		
		auto red = glm::vec3(1., 0., 0.);
		auto blue = glm::vec3(0., 1., 0.);
		auto clearColor = glm::mix(red, blue, zigzag(mixValue));

		glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// bind if another object was drawn previously;
		trollcake.bind();
		derpina.bind();
//		glBindTexture(GL_TEXTURE_2D, id);
//		glBindVertexArray(VAO);
//		glBindBuffer(GL_ARRAY_BUFFER, VBO);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(GL_TRIANGLES, std::size(squareIdxs), GL_UNSIGNED_INT, nullptr);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}
#endif
