#pragma once
#include "Core/Base.h"

namespace ZuneCraft {
	struct GLShader {
		GLuint Id;
	};

	struct GLBuffer {
		GLuint Id;
		GLuint VertexArray;
		GLenum Type;
		GLuint AttributeCount;
		GLuint Size;
	};

	struct GLColorAttachement {
		GLuint Id;
		GLenum InternalFormat;
		GLenum Format;
		GLenum Type;
	};

	struct GLDepthAttachement {
		GLuint Id;
		GLenum Type;
	};

	struct GLRenderTarget {
		GLuint Id;
		GLuint Width;
		GLuint Height;
		GLDepthAttachement DepthAttachement;
		std::vector<GLColorAttachement> ColorAttachements;
	};

	struct GLTexture {
		GLuint Id;
		GLuint Width;
		GLuint Height;
		GLenum Format;
		GLenum DataType;
	};
}