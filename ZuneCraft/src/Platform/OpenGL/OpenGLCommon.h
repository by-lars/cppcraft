#pragma once
#include "Core/Base.h"
#include "Graphics/DeviceResources.h"
#include <string>
#include <vector>

namespace ZuneCraft {
	struct GLShader {
		GLuint Id;
	};

	struct GLBuffer {
		GLuint Id;
		GLuint VertexArray;
		GLenum Type;
		GLuint AttributeCount;
		GLuint Stride;
		GLuint Count;
	};

	struct GLAttachement {
		GLuint Id;
		GLenum Format;
		GLenum Type;
	};

	struct GLRenderTarget {
		GLuint Id;
		GLuint Width;
		GLuint Height;
		std::vector<GLAttachement> Attachements;
	};

	struct GLTexture {
		GLuint Id;
		GLuint Width;
		GLuint Height;
		GLenum Format;
		GLenum DataType;
	};

	struct GLUniform {
		GLuint ProgramId;
		GLsizei Count;
		StorageFormat Format;
		GLint Location;
		std::string Name;
	};
}