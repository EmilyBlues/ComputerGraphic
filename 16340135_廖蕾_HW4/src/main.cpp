#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iostream>
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

using namespace std;
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const char *vertexShaderSource = "#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec3 color;\n"
"out vec3 ourColor;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"	gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"	ourColor = color;\n"
"}\n";

const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor; \n"
"in vec3 ourColor; \n"
"void main()\n"
"{\n"
"	FragColor = vec4(ourColor, 1.0); \n"
"}\n";

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

	// glad init
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}

	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// fragment shader
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	// link shaders
	int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Setup ImGui bindings
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	ImGui_ImplGlfwGL3_Init(window, true);
	ImGui::StyleColorsDark();

	//default enable depth
	glEnable(GL_DEPTH_TEST);

	float vertices[216] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,

		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f
	};
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	bool depth = true;
	bool rotation = false;
	bool scaling = false;
	bool bonus = false;

	float translation_x = 0.0;
	float translation_y = 0.0;

	float scale_x = 1.0;
	float scale_y = 1.0;
	float scale_z = 1.0;

	while (!glfwWindowShouldClose(window))
	{
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
			ImGui::Text("Transformation: ");
			ImGui::PushItemWidth(50);
			ImGui::SliderFloat("X: ", &translation_x, -1.0f, 1.0f);
			ImGui::SliderFloat("Y: ", &translation_y, -1.0f, 1.0f);
			ImGui::Checkbox("Depth", &depth);
			ImGui::Checkbox("Rotation", &rotation);
			ImGui::Checkbox("Scaling", &scaling);
			ImGui::Checkbox("Bonus", &bonus);

			glBindBuffer(GL_ARRAY_BUFFER, VAO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			// position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			// texture coord attribute
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			// activate shader
			glUseProgram(shaderProgram);

			// create transformations
			glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
			glm::mat4 view = glm::mat4(1.0f);
			glm::mat4 projection = glm::mat4(1.0f);

			view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f));
			view = glm::rotate(view, glm::radians(30.0f), glm::vec3(1.0f, -1.0f, 0.0f));
			projection = glm::perspective(glm::radians(60.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

			// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
			string name = "view";
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(view));
			name = "projection";
			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(projection));

			if (!bonus) {
				if (rotation) {
					model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(1.0f, 0.0f, 1.0f));
				}
				if (scaling) {
					ImGui::SliderFloat("ScaleX", &scale_x, 0.5f, 2.0f);
					ImGui::SliderFloat("ScaleY", &scale_y, 0.5f, 2.0f);
					ImGui::SliderFloat("ScaleZ", &scale_z, 0.5f, 2.0f);
					model = glm::scale(model, (abs(sin((float)glfwGetTime())) + 0.1f) * glm::vec3(scale_x, scale_y, scale_z));
				}
				model = glm::translate(model, glm::vec3(translation_x, translation_y, 0.0f));
				model = glm::scale(model, glm::vec3(scale_x, scale_y, scale_z));
				name = "model";
				glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
			else
			{
				float radius = 2.0f;
				float camX = sin(glfwGetTime()) * radius;
				float camZ = cos(glfwGetTime()) * radius;

				model = glm::translate(model, glm::vec3(camX, 0.0f, camZ));
				model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::scale(model, 0.5f * glm::vec3(1.0f, 1.0f, 1.0f));

				name = "model";
				glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 36);

				model = glm::mat4(1.0f);
				model = glm::rotate(model, (float)glfwGetTime()*0.5f, glm::vec3(0.0f, 1.0f, 0.0f));

				name = "model";
				glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
			if (depth) {
				glEnable(GL_DEPTH_TEST);
			}
			else {
				glDisable(GL_DEPTH_TEST);
			}
		}
		
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glUseProgram(shaderProgram);
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	// press ESC to close
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}