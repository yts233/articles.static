#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <shader.h>
#include <stb_image.h>
#include <iostream>

int w = 800, h = 600;
struct Camera {
	glm::vec3 position;
	glm::vec3 up = glm::vec3(0.0F, 1.0F, 0.0F);
	float yaw;
	float pitch;
	float fovy;

	glm::vec3 direction;
	glm::vec3 front;
	glm::vec3 right;

	void calculate() {
		if (pitch > 89.9F)pitch = 89.9F;
		else if (pitch < -89.9F)pitch = -89.9F;
		direction.x = glm::cos(glm::radians(pitch)) * glm::cos(glm::radians(yaw));
		direction.y = glm::sin(glm::radians(pitch));
		direction.z = glm::cos(glm::radians(pitch)) * glm::sin(glm::radians(yaw));
		right = glm::normalize(glm::cross(direction, up));
		front.x = glm::cos(glm::radians(yaw));
		front.z = glm::sin(glm::radians(yaw));
	}

	glm::mat4 calculateViewMatrix() {
		return glm::lookAt(position, position + direction, up);
	}
	glm::mat4 calculateProjectionMatrix() {
		return glm::perspective(glm::radians(fovy), (float)w / ((float)h + 0.01f), 0.1f, 100.0f);
	}
};

unsigned int cubeVAO;
const float cubeVertices[180] = {
	//	  顶点坐标		纹理坐标		
	// front
	1.0f, 0.0f, 0.0f,	0.0f, 0.0f,
	0.0f, 0.0f, 0.0f,	1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,	1.0f, 1.0f,
	0.0f, 1.0f, 0.0f,	1.0f, 1.0f,
	1.0f, 1.0f, 0.0f,	0.0f, 1.0f,
	1.0f, 0.0f, 0.0f,	0.0f, 0.0f,

	// back
	0.0f, 0.0f, 1.0f,	0.0f, 0.0f,
	1.0f, 0.0f, 1.0f,	1.0f, 0.0f,
	1.0f, 1.0f, 1.0f,	1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,	1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,	0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,	0.0f, 0.0f,

	// left
	0.0f, 1.0f, 1.0f,	1.0f, 1.0f,
	0.0f, 1.0f, 0.0f,	0.0f, 1.0f,
	0.0f, 0.0f, 0.0f,	0.0f, 0.0f,
	0.0f, 0.0f, 0.0f,	0.0f, 0.0f,
	0.0f, 0.0f, 1.0f,	1.0f, 0.0f,
	0.0f, 1.0f, 1.0f,	1.0f, 1.0f,

	// right
	1.0f, 0.0f, 0.0f,	1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,	1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,	0.0f, 1.0f,
	1.0f, 1.0f, 1.0f,	0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,	0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,	1.0f, 0.0f,

	// bottom
	0.0f, 0.0f, 0.0f,	0.0f, 1.0f,
	1.0f, 0.0f, 0.0f,	1.0f, 1.0f,
	1.0f, 0.0f, 1.0f,	1.0f, 0.0f,
	1.0f, 0.0f, 1.0f,	1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,	0.0f, 0.0f,
	0.0f, 0.0f, 0.0f,	0.0f, 1.0f,

	// top
	0.0f, 1.0f, 1.0f,	0.0f, 0.0f,
	1.0f, 1.0f, 1.0f,	1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,	1.0f, 1.0f,
	1.0f, 1.0f, 0.0f,	1.0f, 1.0f,
	0.0f, 1.0f, 0.0f,	0.0f, 1.0f,
	0.0f, 1.0f, 1.0f,	0.0f, 0.0f
};

struct Cube {
	static void initalize() {
		glGenVertexArrays(1, &cubeVAO);
		glBindVertexArray(cubeVAO);
		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	/// <summary>每个面的纹理</summary>
	int faces[6];

	/// <summary>渲染一个面</summary>
	/// <param name="face">方块的一个面,0:Front,1:Back,2:Left,3:Right,4:Bottom,5:Top</param>
	void render(int face) {
		glBindTexture(GL_TEXTURE_2D, faces[face]);
		glDrawArrays(GL_TRIANGLES, face * 6, 6);
	}

	/// <summary>渲染所有面</summary>
	void render() {
		for (int i = 0; i < 6; i++)
			render(i);
	}

	/// <summary>绑定</summary>
	void bind() {
		glBindVertexArray(cubeVAO);
	}

};

float targetYaw, targetPitch, mouseSensitivity = 0.1F;
Camera camera;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void loadTexture(const std::string filename, unsigned int* texture, bool nearest);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);

int main()
{
	glfwInit(); //初始化glfw
	// 配置glfw
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);

	// initalization
	glClearColor(0.3F, 0.4F, 0.7F, 1.0F);
	glEnable(GL_DEPTH_TEST);

	Shader ourShader("shader.vs", "shader.fs");

	Cube::initalize();

	unsigned int texture;
	loadTexture("huaji.png", &texture, false);

	Cube huaji;
	huaji.faces[0] = texture;
	huaji.faces[1] = texture;
	huaji.faces[2] = texture;
	huaji.faces[3] = texture;
	huaji.faces[4] = texture;
	huaji.faces[5] = texture;

	camera.fovy = 45.0F;
	camera.position = glm::vec3(0.0F, 0.0F, -1.0F);

	ourShader.use();
	ourShader.setMat4("model", glm::mat4(1.0F));
	ourShader.setMat4("view", glm::mat4(1.0F));
	ourShader.setMat4("projection", glm::mat4(1.0F));

	while (!glfwWindowShouldClose(window)) { //render loop

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ourShader.use(); // 激活着色器

		float deltaYaw = targetYaw - camera.yaw;
		float deltaPitch = targetPitch - camera.pitch;
		if (glm::abs(deltaYaw) < 0.01F)
			camera.yaw = targetYaw;
		else camera.yaw += deltaYaw * 0.7F;
		if (glm::abs(deltaPitch) < 0.01F)
			camera.pitch = targetPitch;
		else camera.pitch += deltaPitch * 0.7F;

		camera.calculate();
		ourShader.setMat4("view", camera.calculateViewMatrix());
		ourShader.setMat4("projection", camera.calculateProjectionMatrix());

		huaji.bind();
		huaji.render();

		glfwSwapBuffers(window);
		glfwPollEvents();

		processInput(window);
	}

	glfwTerminate();

	return 0;
}

bool firstMouse = false;
float lastX, lastY;
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) // 这个bool变量初始时是设定为true的
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
		return;
	}
	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos; // 注意这里是相反的，因为y坐标是从底部往顶部依次增大的
	lastX = (float)xpos;
	lastY = (float)ypos;

	targetYaw += xoffset * mouseSensitivity;
	targetPitch += yoffset * mouseSensitivity;
	if (targetPitch > 89.9F)
		targetPitch = 89.9F;
	else if (targetPitch < -89.9F)
		targetPitch = -89.9F;
}

float lastFrame = 0;
// 处理输入
void processInput(GLFWwindow* window)
{
	float currentFrame = (float)glfwGetTime();
	float deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	float speed = 5.0F * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.position += camera.front * speed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.position -= camera.front * speed;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.position -= camera.right * speed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.position += camera.right * speed;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.position += glm::vec3(0.0f, 1.0f, 0.0f) * speed;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.position -= glm::vec3(0.0f, 1.0f, 0.0f) * speed;
}

// 窗口大小改变回调
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	w = width;
	h = height;
	glViewport(0, 0, width, height);
}

// 加载纹理
void loadTexture(const std::string filename, unsigned int* texture, bool nearest) {
	glGenTextures(1, texture);
	glBindTexture(GL_TEXTURE_2D, *texture);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	if (nearest) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	//stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}