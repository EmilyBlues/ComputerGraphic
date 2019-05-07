#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include "camera.h"
#include "shader.h"

using namespace std;
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main() {
	glfwInit();
	// OpenGL version & mode setting
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a window & context/viewpoint setting
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Homework3", NULL, NULL);
	if (window == NULL) {
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad init
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}
	
	Shader shader("vShaderSrc.txt", "fShaderSrc.txt");
	Shader Gshader("GouraudV.txt", "GouraudF.txt");
	Shader lightShader("lightsShaderSrc.txt", "lightfShaderSrc.txt");

	// Setup ImGui bindings
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsDark();

	//default enable depth
	glEnable(GL_DEPTH_TEST);

	ImVec4 tri1 = ImVec4(1.0f, 0.0f, 0.0f, 1.00f);
	ImVec4 tri2 = ImVec4(0.0f, 1.0f, 0.0f, 1.00f);
	ImVec4 tri3 = ImVec4(0.0f, 0.0f, 1.0f, 1.00f);
	ImVec4 tri4 = ImVec4(0.5f, 0.f, 0.5f, 1.00f);
	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	unsigned int cubeVAO, VBO, lightVAO;
	// cube VAO&VBO
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(cubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// light VAO &VBO
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	int radioMark = 0;

	glm::vec3 lightPosition = glm::vec3(1.2f, 1.0f, 2.0f);
	glm::vec3 viewPosition = glm::vec3(6.0f, 2.0f, 5.0f);
	float ambientStrength = 0.3;
	float specularStrength = 0.9;
	float concentrate = 32;
	float diffuseMutiple = 0.8;

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// input
		// -----
		processInput(window);
		glfwPollEvents();
		// render
		// ------
		ImGui_ImplGlfwGL3_NewFrame();
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now
		
		{
			ImGui::RadioButton("Phong", &radioMark, 0);
			ImGui::RadioButton("Gouraud", &radioMark, 1);
			ImGui::RadioButton("Bonus", &radioMark, 2);
			ImGui::SliderFloat("Ambient", &ambientStrength, 0.0f, 1.0f);
			ImGui::SliderFloat("Diffuse", &diffuseMutiple, 0.0f, 1.0f);
			ImGui::SliderFloat("Specular", &specularStrength, 0.0f, 3.0f);
			ImGui::SliderFloat("Shininess", &concentrate, 2.0f, 128.0f);
			ImGui::DragFloat3("Light POS", &lightPosition.x, 2, 0);
			// create transformations
			glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
			glm::mat4 view = glm::mat4(1.0f);
			glm::mat4 projection = glm::mat4(1.0f);
			view = glm::lookAt(viewPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			projection = glm::perspective(20.0f, 1.0f, 0.01f, 100.0f);
			float radius = 20.0f;
			float camX = sin(glfwGetTime()) * radius;
			float camZ = cos(glfwGetTime()) * radius;
			if (radioMark == 0 || radioMark == 2) {
				if (radioMark == 2) {
					lightPosition = glm::vec3(camX, lightPosition.y, camZ);
				}
				shader.use();
				shader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
				shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
				model = glm::rotate(model, 30.0f, glm::vec3(0, 1, 0));
				shader.setVec3("lightPos", lightPosition.x, lightPosition.y, lightPosition.z);
				shader.setVec3("viewPos", viewPosition.x, viewPosition.y, viewPosition.z);
				shader.setFloat("ambientStrength", ambientStrength);
				shader.setFloat("diffuseMutiple", diffuseMutiple);
				shader.setFloat("specularStrength", specularStrength);
				shader.setFloat("concentrate", concentrate);
				shader.setMat4("model", model);
				shader.setMat4("view", view);
				shader.setMat4("projection", projection);
			}
			else if (radioMark == 1) {
				Gshader.use();
				Gshader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
				Gshader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
				model = glm::rotate(model, 30.0f, glm::vec3(0, 1, 0));
				Gshader.setVec3("lightPos", lightPosition.x, lightPosition.y, lightPosition.z);
				Gshader.setVec3("viewPos", viewPosition.x, viewPosition.y, viewPosition.z);
				Gshader.setFloat("ambientStrength", ambientStrength);
				shader.setFloat("diffuseMutiple", diffuseMutiple);
				Gshader.setFloat("specularStrength", specularStrength);
				Gshader.setFloat("concentrate", concentrate);
				Gshader.setMat4("model", model);
				Gshader.setMat4("view", view);
				Gshader.setMat4("projection", projection);
			}
			glBindVertexArray(cubeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			lightShader.use();
			model = glm::mat4();
			model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
			model = glm::translate(model, lightPosition);
			lightShader.setMat4("model", model);
			lightShader.setMat4("view", view);
			lightShader.setMat4("projection", projection);
			glBindVertexArray(lightVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
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

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}

void processInput(GLFWwindow* window) {
	// press ESC to close
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

}