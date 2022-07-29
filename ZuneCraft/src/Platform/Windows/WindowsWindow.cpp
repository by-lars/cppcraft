#include "Platform/Windows/WindowsWindow.h"
#include "Core/Base.h"
#include "Graphics/Renderer.h"
#include "Core/Game.h"

#include <glad/gl.h>

#include <iostream>


namespace ZuneCraft {

    void APIENTRY glDebugOutput(GLenum source,
        GLenum type,
        unsigned int id,
        GLenum severity,
        GLsizei length,
        const char* message,
        const void* userParam)
    {
        // ignore non-significant error/warning codes
        if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

        ZC_ERROR("---------------");
        ZC_ERROR("Debug message (" << id << "): " << message);

        switch (source)
        {
        case GL_DEBUG_SOURCE_API:             ZC_ERROR("Source: API"); break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   ZC_ERROR("Source: Window System"); break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: ZC_ERROR("Source: Shader Compiler"); break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     ZC_ERROR("Source: Third Party"); break;
        case GL_DEBUG_SOURCE_APPLICATION:     ZC_ERROR("Source: Application"); break;
        case GL_DEBUG_SOURCE_OTHER:           ZC_ERROR("Source: Other"); break;
        } 

        switch (type)
        {
        case GL_DEBUG_TYPE_ERROR:               ZC_ERROR("Type: Error"); break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: ZC_ERROR("Type: Deprecated Behaviour"); break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  ZC_ERROR("Type: Undefined Behaviour"); break;
        case GL_DEBUG_TYPE_PORTABILITY:         ZC_ERROR("Type: Portability"); break;
        case GL_DEBUG_TYPE_PERFORMANCE:         ZC_ERROR("Type: Performance"); break;
        case GL_DEBUG_TYPE_MARKER:              ZC_ERROR("Type: Marker"); break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          ZC_ERROR("Type: Push Group"); break;
        case GL_DEBUG_TYPE_POP_GROUP:           ZC_ERROR("Type: Pop Group"); break;
        case GL_DEBUG_TYPE_OTHER:               ZC_ERROR("Type: Other"); break;
        } 

        switch (severity)
        {
        case GL_DEBUG_SEVERITY_HIGH:         ZC_ERROR("Severity: high"); break;
        case GL_DEBUG_SEVERITY_MEDIUM:       ZC_ERROR("Severity: medium"); break;
        case GL_DEBUG_SEVERITY_LOW:          ZC_ERROR("Severity: low"); break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: ZC_ERROR("Severity: notification"); break;
        } 
        
    }

	WindowsWindow::WindowsWindow() {
        if (glfwInit() != 1) { ZC_FATAL_ERROR("Could not initialize GLFW"); }

		m_Height = 272 * 4;
		m_Width = 480 * 4;
		
        RenderAPI::API api = Game::Get().GetConfig().GraphicsAPI;
        

        //glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


		#ifdef DEBUG
		glfwWindowHint(GLFW_CONTEXT_DEBUG, GLFW_TRUE);
        #endif

		m_Window = glfwCreateWindow(m_Width, m_Height, "ZuneCraft", NULL, NULL);
		glfwMakeContextCurrent(m_Window);

        int version = gladLoadGL(glfwGetProcAddress);
        if (version == 0) {
            ZC_FATAL_ERROR("Could not load OpenGL 4.6");
            return;
        }

        #ifdef DEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        #endif

        glfwSetFramebufferSizeCallback(m_Window, WindowsWindow::OnWindowResized);
        glfwSwapInterval(1);
        
    }

    void WindowsWindow::OnWindowResized(GLFWwindow* window, int width, int height) {
        if (width > 0 && height > 0) {
            Renderer::SetResolution(width, height);
        }
    }

	bool WindowsWindow::ShouldClose() {
		return glfwWindowShouldClose(m_Window);
	}

	void* WindowsWindow::GetNativeWindow() {
		return m_Window;
	}

	void WindowsWindow::Update() {
        static double lastTime = 0;
        double frameTime = glfwGetTime();
        double frameDelta = frameTime - lastTime;

        std::string title("ZuneCraft - FPS: ");
        title += std::to_string(int(1 / frameDelta));

        glfwSetWindowTitle(m_Window, title.c_str());

        lastTime = frameTime;

		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	WindowsWindow::~WindowsWindow() {
		glfwTerminate();
	}
}