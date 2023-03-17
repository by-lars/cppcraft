#pragma once
#include "Core/Base.h"
#include "Graphics/RenderAPI.h"
#include <string>

namespace ZuneCraft {
	class GLShader : public Shader {
	public:
		GLShader(const std::string& shaderName);
		~GLShader();
		void Bind() override;
		uint32_t GetNativeHandle() override;

		void Set(const std::string& name, int value) override;
		void Set(const std::string& name, float value) override;
		void Set(const std::string& name, const glm::vec3& value) override;
		void Set(const std::string& name, const glm::vec4& value) override;
		void Set(const std::string& name, const glm::mat4& value) override;
		void Set(const std::string& name, float* array, size_t size) override;

	private:
		void CompileShader(GLint shader, const GLchar* source);
		void ShaderFromSource(const std::string& assetName);
		void ShaderFromBinary(const std::string& assetName);

		GLuint m_Program;
	};
}