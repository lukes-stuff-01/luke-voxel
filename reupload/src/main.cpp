#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <shader.hpp>
#include <camera.hpp>
#include <stdio.h>
#define CHUNK_SIZE 32
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

//variables related to switching between render types
#define BINARY_VOXEL 0
#define BINARY_VOX_IMAGE 1


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void renderVoxel(Shader* shaders, unsigned int* VBO, unsigned int* offsetsVBO, glm::mat4* model, glm::mat4* view, glm::mat4* projection);
void renderFromImage(Shader* shaders, unsigned int* VBO, unsigned int* offsetsVBO, glm::mat4* model, glm::mat4* view, glm::mat4* projection, unsigned int * texture);
unsigned int loadTexture(char const* path);
float deltaTime = 0.0f;
float lastFrame = 0.0f;
glm::vec3* generateVoxelField();

Camera camera(glm::vec3(16.0f, CHUNK_SIZE/2, -CHUNK_SIZE/2));

float lastX = 400, lastY = 300; // god I hate declaring global variables I feel sick
bool firstMouse = true;
float imageRotationType = 0;
float threshold = 0.5f;
bool flipX = false;
bool flipY = false;
bool flipZ = false;
int renderType = BINARY_VOX_IMAGE;//renderType variable is used to tell the main loop which function to execute which determines the type of rendering being done.
int fileSize = CHUNK_SIZE;
std::string fileName = "test_image.png";

//

int main(int argc, char* argv[]) {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	
	GLFWwindow* window = glfwCreateWindow(800, 600, "Luke-Voxel", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {//passes GLAD the os specific opengl functions
		std::cout << "GLAD failed loading" << std::endl;
		return -1;
	}
	
	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	float vertices[] = {
		// positions          // normals           // texture coords
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};//copy pasted vertex data not writing out 36 vertices

	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	//Video buffer object shit
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//put triangle into vbo
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	//telling opengl where the triangles are.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);//position 0 is vertices

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);//position 1 is normals

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);//position 2 is tex coords
	glm::vec3* offsets = generateVoxelField();
	unsigned int offsetsVBO;
	glGenBuffers(1, &offsetsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, offsetsVBO);
	glBufferData(GL_ARRAY_BUFFER, CHUNK_SIZE* CHUNK_SIZE* CHUNK_SIZE * sizeof(glm::vec3), &offsets[0], GL_DYNAMIC_DRAW);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,  sizeof(glm::vec3), (void*)(0));
	glVertexAttribDivisor(3, 1);//set to 1 so that it ticks forward every instance
	glEnableVertexAttribArray(3);//I fucking forgot to enable the array
	

	Shader shaders(RESOURCES_PATH "shader.vs", RESOURCES_PATH "shader.fs");//can add geometry shader into the function
	Shader shaders_image(RESOURCES_PATH "shader_i.vs", RESOURCES_PATH "shader_i.fs");
	shaders.use();
	glm::mat4 model = glm::mat4(1.0f);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	
	unsigned int texture = 0;
	if (argc == 3) {//argc will be 3 when there are 2 inputs, which is what we take. If theres more we assume it's invalid and load default.
		fileName = (std::string)argv[1];
		fileSize = std::stoi((std::string)argv[2]);
	};
	camera.position = glm::vec3(fileSize / 2, fileSize / 2, -fileSize / 2);

	if (argc == 3)
	{
		texture = loadTexture(fileName.c_str());
	}
	else {
		texture = loadTexture(RESOURCES_PATH "test_image.png");
	};
	
	while (!glfwWindowShouldClose(window)) {
		
		processInput(window);
		glClearColor(0.1f, 0.4f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		model = glm::mat4(1.0f);
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		/*for (int i = 0; i < 32; i++) {
			for (int j = 0; j < 32; j++) {
				for (int k = 0; k < 32; k++)
				{
					model = glm::translate(glm::mat4(1.0f), glm::vec3(i * 4, j * 4, k * 4));
					shaders.setMat4("model", model);
					glDrawArrays(GL_TRIANGLES, 0, 36);
				}
			}
		}*/
		switch (renderType) {
		case BINARY_VOXEL:
			renderVoxel(&shaders, &VBO, &offsetsVBO, &model, &view, &projection);
			break;
		case BINARY_VOX_IMAGE:
			renderFromImage(&shaders_image, &VBO, &offsetsVBO, &model, &view, &projection, &texture);
			break;
		default:
			renderVoxel(&shaders, &VBO, &offsetsVBO, &model, &view, &projection);//in case of an error, just render binary voxels
			break;
		}
		float currentFrame = glfwGetTime();//wait, if this value overflows, then it will be 0 meaning that deltatime will become a huge negative value, that could be bad, implement some sort of protection against this?
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	glfwTerminate();
	delete offsets;
	return 0;
}

void renderVoxel(Shader* shaders, unsigned int* VBO, unsigned int* offsetsVBO, glm::mat4* model, glm::mat4* view, glm::mat4* projection) {
	shaders->use();
	shaders->setMat4("view", *view);
	shaders->setMat4("projection", *projection);
	shaders->setMat4("model", *model);
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBindBuffer(GL_ARRAY_BUFFER, *offsetsVBO);

	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void renderFromImage(Shader* shaders, unsigned int* VBO, unsigned int* offsetsVBO, glm::mat4* model, glm::mat4* view, glm::mat4* projection, unsigned int* texture) {
	shaders->use();
	shaders->setMat4("view", *view);
	shaders->setMat4("projection", *projection);
	shaders->setMat4("model", *model);
	shaders->setInt("size", fileSize);
	shaders->setFloat("threshold", threshold);
	shaders->setInt("orientation", (int)floor(imageRotationType) % 6);
	shaders->setVec3("flipSelect", flipX, flipY, flipZ);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *texture);

	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, fileSize * fileSize * fileSize);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

glm::vec3* generateVoxelField() {
	glm::vec3* offsets = new glm::vec3[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
	unsigned long offset_count = 0;
	for (int i = 0; i < CHUNK_SIZE; i++) {
		for (int j = 0; j < CHUNK_SIZE; j++) {
			for (int k = 0; k < CHUNK_SIZE; k++) {
				if (pow(i - CHUNK_SIZE/2, 2) + pow((j - CHUNK_SIZE/2), 2) + pow((k - CHUNK_SIZE/2), 2) <= pow(CHUNK_SIZE/2, 2))
				{
					offsets[offset_count] = glm::vec3(i, j, k);//I have no clue why I don't have to dereference this.
					offset_count++;
				}
				else {
					offsets[offset_count] = glm::vec3(-1,-1,-1);//I have no clue why I don't have to dereference this.
					offset_count++;
				}
			}
		}
	};
	return offsets;
}


unsigned int loadTexture(char const* path) {
	unsigned int texture;
	glGenTextures(1, &texture);
	// dey call me da filta, mr bilinear filta(filtering for mipmaps)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	glBindTexture(GL_TEXTURE_2D, texture);
	if (data) {
		if (nrChannels == 1) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
		}
		else if (nrChannels == 3) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else if (nrChannels == 4) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Textures failed to load." << std::endl;
		glfwTerminate();
		return -1;
	};
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//these are for rendering if it's a lil off and cant be rendered perfectly
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	return texture;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) 
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
	const float cameraSpeed = 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_PERIOD && action == GLFW_PRESS && imageRotationType + 1 != 6)
	{
		imageRotationType += 1;
	}
	if (key == GLFW_KEY_COMMA && action == GLFW_PRESS && imageRotationType - 1 != -1)
	{
		imageRotationType -= 1;
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		renderType = (renderType + 1) % 2;
	}
	if (key == GLFW_KEY_1 && action == GLFW_PRESS) {
		flipX = !flipX;
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		flipY = !flipY;
	}
	if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
		flipZ = !flipZ;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse == true) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xOffset = xpos - lastX;
	float yOffset = ypos - lastY;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	//camera.ProcessMouseScroll((float)yoffset);
	if (threshold + yoffset / 100 <= 1.0f && threshold + yoffset / 100 >= 0.0f){
			threshold += yoffset / 100;
	}
}