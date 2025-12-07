#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "tools/Polygon.h"
#include "tools/Cube.h"
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include "stb_image.h"

using namespace std;
using namespace glm;

int SCR_WIDTH = 1280;
int SCR_HEIGHT = 720;

Camera camera(vec3(0.0f, 0.0f, 4.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

vec3 firstLight(0.0f, 0.0f, 0.0f);
vec3 lightPos(0.0f, 0.0f, -2.5f);

bool blinn = true;


void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

vector<GLuint> loadTextures(vector<string> paths, GLuint wrapOption = GL_REPEAT, GLuint filterOption = GL_LINEAR) {
	vector<GLuint> textures = {};

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapOption);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapOption);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterOption);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterOption);

	for (string path : paths)
	{
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		int width, height, nrChannels;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);


		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
			std::cout << "Failed to load texture" << std::endl;
		stbi_image_free(data);

		textures.push_back(texture);
	}

	return textures;
}

int main()
{
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Solar System", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);


	Shader allShader("./shaders/vs/L5.vs", "./shaders/fs/L5-Multi.fs");
	Shader lightSourceShader("./shaders/vs/LightSource.vs", "./shaders/fs/LightSource-Texture.fs");

	glEnable(GL_DEPTH_TEST);

	stbi_set_flip_vertically_on_load(true);

	vector<string> texturePaths = {};
	texturePaths.push_back("./textures/earth.jpeg");
	texturePaths.push_back("./textures/mars.jpeg");
	texturePaths.push_back("./textures/sun.jpeg");
	vector<GLuint> textures = loadTextures(texturePaths);
	Model earthPlanet("./models/Sphere.glb");
	Model marsPlanet("./models/Sphere.glb");
	Model sun("./models/Sphere.glb");

	Cube LightSource(firstLight, 0.2f, vec3(1.0f, 1.0f, 1.0f));

	bool BPressed = false;

	while (!glfwWindowShouldClose(window))
	{
		allShader.use();
		allShader.setBool("blinn", true);
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		float time = (float)glfwGetTime();

		processInput(window);

		glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
		//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mat4 projection = mat4(1.0f);
		projection = perspective(radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		allShader.setMat4("projection", projection);
		

		mat4 view = mat4(1.0f);
		view = view = camera.GetViewMatrix();
		allShader.setMat4("view", view);

		mat4 trans = mat4(1.0f);
		trans = rotate(trans, time, vec3(1.0f, 0.0f, 1.0f));

		/*vec4 rotatedVectorHomogeneous = trans * vec4(firstLight, 1.0f);
		lightPos = vec3(rotatedVectorHomogeneous);*/

		allShader.setVec3("lightPos", lightPos);
		allShader.setVec3("viewPos", camera.Position);
		allShader.setVec3("objectColor", vec3(1.0f, 1.0f, 1.0f));
		allShader.setMat4("model", mat4(1.0f));

		/*glBindTexture(GL_TEXTURE_2D, textures[0]);
		plainBall.Draw(allShader);*/



		mat4 transformation = mat4(1.0f);
		transformation = glm::rotate(transformation,time,vec3(0.0f,1.0f,0.0f));
		transformation = glm::translate(transformation, vec3(0.0f, 0.0f, 3.0f));
		transformation = glm::scale(transformation, vec3(0.2f, 0.2f, 0.2f));

		

		allShader.setMat4("model", transformation);
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		earthPlanet.Draw(allShader);


		lightSourceShader.use();
		lightSourceShader.setMat4("projection", projection);
		lightSourceShader.setMat4("view", view);
		lightSourceShader.setVec3("objectColor", vec3(1.0f,1.0f,1.0f));
		mat4 sunTransform = mat4(1.0f);
		sunTransform = glm::scale(sunTransform, vec3(0.3f, 0.3f, 0.3f));
		lightSourceShader.setMat4("model", sunTransform);
		glBindTexture(GL_TEXTURE_2D, textures[2]);
		sun.Draw(lightSourceShader);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
