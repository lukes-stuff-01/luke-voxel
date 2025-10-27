#ifndef SHADER_H
#define SHADER_H

#include<glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	unsigned int ID;//id is used for assigning uniforms

	//when you do the construction you input the path and you win and everyone will think you're awesome
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath);
	//these are for the uniforms
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;

	void use() const
	{
		glUseProgram(ID);
	}
	void setVec2(const std::string& name, const glm::vec2 &value) const//holy guacamole !!!!!!!!!! overloaded functions !!!!!!!!!!!
	{//shit code practices I know but I'm too fucking lazy to get overloaded functions working outside of the functions.
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}

	void setVec2(const std::string & name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	}
	
	void setVec3(const std::string & name, const glm::vec3 & value) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec3(const std::string & name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}
	
	void setVec4(const std::string & name, const glm::vec4 & value) const
	{
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec4(const std::string & name, float x, float y, float z, float w) const
	{
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}
	
	void setMat2(const std::string & name, const glm::mat2 & mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	
	void setMat3(const std::string & name, const glm::mat3 & mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	
	void setMat4(const std::string & name, const glm::mat4 & mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	
};
Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = NULL) {
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;

	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;
	

	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	if (geometryPath != NULL)
	{
		gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	}
	try
	{
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		if (geometryPath != NULL)
		{
			gShaderFile.open(geometryPath);
		}
		std::stringstream vShaderStream, fShaderStream, gShaderStream;
		//rdbuf means read buffer and you are putting it into the shader streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		if (geometryPath != NULL)
		{
			gShaderStream << gShaderFile.rdbuf();
		}
		//the time has come, execute order 71
		vShaderFile.close();
		fShaderFile.close();
		if (geometryPath != NULL)
		{
			gShaderFile.close();
		}

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		if (geometryPath != NULL)
		{
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::ifstream::failure e) {
		std::cout << "shader read error occured. " << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	const char* gShaderCode = geometryCode.c_str();

	//she vertex on my shader til I render
	unsigned int vertex, fragment, geometry;
	vertex = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);


	//check your(vertex shader)self before you wreck your(vertex shader)self
	int success = 0;
	char infolog[512];

	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infolog);
		std::cout << "Vertex shader compile failed: " << infolog << std::endl;
	}

	//she fragment on my shader til I colour
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	//[insert stupid joke about fragment shaders and examining them idk man fuckin uhh]
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infolog);
		std::cout << "Compiling issue with fragment shader: " << infolog << std::endl;
	}

	if (geometryPath != NULL)
	{
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);

		//checking geometry shader for errors
		glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(geometry, 512, NULL, infolog);
			std::cout << "Geometry shader failed to compile" << infolog << std::endl;
		}
	}

	//shader program more like, shader BROgram, or something, idfk.				  
	ID = glCreateProgram();

	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	if (geometryPath != NULL)
	{
		glAttachShader(ID, geometry);
	}
	glLinkProgram(ID);


	//program check
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infolog);
		std::cout << "Shader program failed to compile, potential attachment issues: " << infolog << std::endl;
	}
	//all systems go so fuckin send that shit, even if it aint work still send it, never give up always send it
	glDeleteShader(vertex);//the time has come, execute order 69
	glDeleteShader(fragment);
	if (geometryPath != NULL)
	{
		glDeleteShader(geometry);
	}
}
															
void Shader::setBool(const std::string& name, bool value) const 
{	
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}																
void Shader::setInt(const std::string& name, int value) const	
{																
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}																
void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
#endif