#include "Platform/OpenGL/GLShader.h"
#include "Platform/OpenGL/GL.h"
#include "Utility/Asset.h"
#include "Graphics/RenderAPI.h"
#include <vector>
#include <glm/gtc/type_ptr.hpp>

namespace ZuneCraft {

	void GLShader::CompileShader(GLint shader, const GLchar* source) {
		glShaderSource(shader, 1, &source, 0);
		glCompileShader(shader);

		GLint success = false;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (success == false) {
			char log[2048];
			glGetShaderInfoLog(shader, 2048, 0, log);
			ZC_FATAL_ERROR("Failed to compile shader: " << shader << "\n" << log);
		}
	}

	GLShader::GLShader(const std::string& assetName) {
		std::string vertexCode = Asset::GetShaderSource(assetName + ".vs");
		std::string fragmentCode = Asset::GetShaderSource(assetName + ".fs");

		m_Program = glCreateProgram();
		GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);

		CompileShader(vertex, vertexCode.c_str());
		CompileShader(fragment, fragmentCode.c_str());

		glAttachShader(m_Program, vertex);
		glAttachShader(m_Program, fragment);

		if (RenderAPI::GetAPI() == RenderAPI::API::OPENGL_ES_2) {
			std::vector<std::string> attributes;
			Asset::GetShaderAttribs(assetName + ".attribs", &attributes);

			for (int i = 0; i < attributes.size(); i++) {
				glBindAttribLocation(m_Program, i, attributes[i].c_str());
			}
		}

		glLinkProgram(m_Program);

		GLint didLink = false;
		glGetProgramiv(m_Program, GL_LINK_STATUS, &didLink);

		if (didLink == false) {
			char log[1024];
			glGetProgramInfoLog(m_Program, 1024, 0, log);
			ZC_FATAL_ERROR("Failed to link shader: " << m_Program << "\n" << log);
		}
		
		ZC_DEBUG("Created GLShader '" << assetName << "'");
	}

	GLShader::~GLShader() {
		glDeleteProgram(m_Program);
	}


	void GLShader::Set(const std::string& name, int value) {
		Bind();
		glUniform1i(glGetUniformLocation(m_Program, name.c_str()), value);
	}

	void GLShader::Set(const std::string& name, float value) {
		Bind();
		glUniform1f(glGetUniformLocation(m_Program, name.c_str()), value);
	}

	void GLShader::Set(const std::string& name, const glm::vec3& value) {
		Bind();
		glUniform3fv(glGetUniformLocation(m_Program, name.c_str()), 1, glm::value_ptr(value));
	}

	void GLShader::Set(const std::string& name, const glm::vec4& value) {
		Bind();
		glUniform4fv(glGetUniformLocation(m_Program, name.c_str()), 1, glm::value_ptr(value));
	}

	void GLShader::Set(const std::string& name, const glm::mat4& value) {
		Bind();
		glUniformMatrix4fv(glGetUniformLocation(m_Program, name.c_str()), 1, false, glm::value_ptr(value));
	}

	void GLShader::Set(const std::string& name, float* array, size_t size) {
		Bind();
		glUniform3fv(glGetUniformLocation(m_Program, name.c_str()), size, array);
	}


	uint32_t GLShader::GetNativeHandle() {
		return m_Program;
	}

	void GLShader::Bind() {
		glUseProgram(m_Program);
	}
}
