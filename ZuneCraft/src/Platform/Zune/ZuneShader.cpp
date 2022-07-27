#include "Graphics/Shader.h"
#include "Core/Base.h"
#include "Core/Logger.h"
#include "Graphics/GL.h"

#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

namespace ZuneCraft {
	GLuint Shader::s_BoundShader = UINT_MAX;

	Result Shader::LoadShaderFromFile(const std::string& path, GLuint shader) {
		ZC_LOG("Loading Shader " << path); 
		std::ifstream file(path.c_str(), std::ios::ate | std::ios::binary);
		
		if(file.is_open() == false) {
			return FAILURE;
		}

		size_t size = file.tellg();
		file.seekg(0);

		char* buffer = new char[size];
		file.read(buffer, size);
		file.close();

		glShaderBinary(1, &shader, GL_NVIDIA_PLATFORM_BINARY_NV, buffer, size);

		delete [] buffer;
		return SUCCESS;
	}

	Shader::Shader(const std::string& shaderName, const std::vector<std::string>& attributes) {
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		m_Id = glCreateProgram();

		std::string vertex = shaderName + ".nvbv";
		std::string fragment = shaderName + ".nvbf";

		if(LoadShaderFromFile(vertex, vertexShader) == FAILURE) {
			ZC_FATAL_ERROR("Could not find Vertex Shader");
		}

		
		if(LoadShaderFromFile(fragment, fragmentShader) == FAILURE) {
			ZC_FATAL_ERROR("Could not find Fragment Shader");
		}
	
		glAttachShader(m_Id, vertexShader);
		glAttachShader(m_Id, fragmentShader);

		for(int i = 0; i < attributes.size(); i++) {
			ZC_DEBUG("Bound " << attributes[i] << " to location " << i);
			glBindAttribLocation(m_Id, i, attributes[i].c_str());
		}

		glLinkProgram(m_Id);
		
		GLint didLink = 0;
		glGetProgramiv(m_Id, GL_LINK_STATUS, &didLink);
		if(didLink == false) {
			char info[1024];
			GLsizei len = 0;
			glGetProgramInfoLog(m_Id, 1024, &len, info);
			ZC_FATAL_ERROR("Could not link shaders:" << info); 
		}

		/*glValidateProgram(m_Id);
		GLint isValidated = 0;
		glGetProgramiv(m_Id, GL_VALIDATE_STATUS, &isValidated);
		if(isValidated == false) {
			char info[1024];
			GLsizei len = 0;
			glGetProgramInfoLog(m_Id, 1024, &len, info);
			ZC_FATAL_ERROR("Could not validate shader program: " << info);
		}*/

		Bind();
	}

	GLuint Shader::GetId() {
		return m_Id;
	}

	int Shader::GetUniform(const std::string& name) {
		return glGetUniformLocation(m_Id, name.c_str());
	}

	void Shader::SetMat4(const std::string& name, const glm::mat4& value) {
		Bind();
		int location = glGetUniformLocation(m_Id, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}

	void Shader::SetVec3(const std::string& name, const glm::vec3& value) {
		Bind();
		int location = glGetUniformLocation(m_Id, name.c_str());
		glUniform3fv(location, 1, glm::value_ptr(value));
	}

	void Shader::SetInt(const std::string& name, int value) {
		Bind();
		int location = glGetUniformLocation(m_Id, name.c_str());
		glUniform1i(location, value);
	}

	Shader::~Shader() {
		glDeleteProgram(m_Id);
	}
	
	void Shader::Bind() {
		if(s_BoundShader != m_Id) {
			glUseProgram(m_Id);
			s_BoundShader = m_Id;
		} 
	}
}
