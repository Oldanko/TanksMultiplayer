#define GLM_SWIZZLE

#include <GL\glew.h>
#include <SOIL\SOIL.h>
#include <glm\glm.hpp>
#include "Window.h"
#include "Sprite.h"
#include "Transform.h"
#include "Tank.h"

#include "UDPClient.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <map>

#include <thread>
#include <chrono>

std::map<uint8_t, Tank*> netObjects;

using namespace glm;

GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);
GLuint Texture(const char * path, GLuint wrap_s, GLuint wrap_t, GLuint min_filter, GLuint mag_filter);

const float speed = 0.05f;

int main()
{
	Window window(960, 540);
	GLuint vao, program;

	glGenVertexArrays(1, &vao);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	program = LoadShaders("resources/shaders/vertex.shader", "resources/shaders/fragment.shader");
	glUseProgram(program);
	glUniform1i(glGetUniformLocation(program, "colormap"), 0);
	glUseProgram(0);
	
	GLuint tankTexture, desertTexture;
	tankTexture = Texture("resources/textures/tank.png", GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	desertTexture = Texture("resources/textures/desert.png", GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
	
	Tank::setTexture(tankTexture);
	Tank tank(glm::vec2(0.0f), 0.0f);
	Tank player2(glm::vec2(10.0f, 10.0f), PI);

	Sprite desert(glm::vec2(0, 24), glm::vec2(0, 12), desertTexture);

	mat3 viewMatrix;
	mat3 perspectiveMatrix = tm_scale(1 / 12.8f, 1 / 7.2f);


	Sleep(1000);
	UDPClient udpclient("127.0.0.1", 8888);
	udpclient.connect();

	tank.update();
	tank.prepareNetData(udpclient);
	udpclient.send();

	std::chrono::high_resolution_clock::time_point time_start, time_now;

	while (window.update())
	{	
		time_start = std::chrono::high_resolution_clock::now();

		viewMatrix = tank.viewMatrix();
		glm::mat3 VP = perspectiveMatrix * viewMatrix;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glUseProgram(program);
		glBindVertexArray(vao);
		glActiveTexture(GL_TEXTURE0);

		tank.draw(VP);
		for (auto o : netObjects)
			o.second->draw(VP);

		glUniformMatrix3fv(0, 1, GL_FALSE, &VP[0][0]);
		desert.draw();

		
		
		udpclient.receive();
		uint8_t num = 0;
		udpclient.read(num);

		for (int i = 0; i < num; i++)
		{
			uint8_t num1;
			udpclient.read(num1);

			if (netObjects.find(num1) == netObjects.end())
				netObjects.emplace(num1, new Tank());

			netObjects[num1]->readNetData(udpclient);
		}

		for (auto o : netObjects)
			o.second->update();

		//if (netObjects.size() > 0)
			//printf("%f\n", glm::distance(tank.position(), netObjects[0]->position()));

		if (glfwGetKey(window.ptr(), GLFW_KEY_W) == GLFW_PRESS)
			tank.move(speed);
		if (glfwGetKey(window.ptr(), GLFW_KEY_S) == GLFW_PRESS)
			tank.move(-speed);

		if (glfwGetKey(window.ptr(), GLFW_KEY_A) == GLFW_PRESS)
			tank.rotate(0.02f);
		if (glfwGetKey(window.ptr(), GLFW_KEY_D) == GLFW_PRESS)
			tank.rotate(-0.02f);

		glBindVertexArray(0);
		glUseProgram(0);

		tank.update();
		tank.prepareNetData(udpclient);
		udpclient.send();

		time_now = std::chrono::high_resolution_clock::now();
		std::this_thread::sleep_for(std::chrono::milliseconds(16) - (time_now - time_start));
	}

	for (auto o : netObjects);
		//delete o.second;

	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(program);

	return 0;
}

//==================================================================================================
GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path)
{
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);

	if (VertexShaderStream.is_open()) {

		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;

		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}
	// Read the Fragment Shader code from the file

	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);

	if (FragmentShaderStream.is_open()) {

		std::string Line = "";

		while (getline(FragmentShaderStream, Line))

			FragmentShaderCode += "\n" + Line;

		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();

	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);

	char const * FragmentSourcePointer = FragmentShaderCode.c_str();

	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);

	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);

	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;

}

GLuint Texture(const char * path, GLuint wrap_s, GLuint wrap_t, GLuint min_filter, GLuint mag_filter)
{
	int width, height;
	unsigned char* image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGBA);

	GLuint textureID;

	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);

	if (wrap_s == GL_CLAMP_TO_BORDER)
	{
		GLfloat borderColor[] = { 1.0, 1.0, 1.0, 0.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);

	glBindTexture(GL_TEXTURE_2D, 0);
	return textureID;
}


