#pragma once

namespace ZuneCraft {
	class Window {
	public:
	//	virtual ~Window() = 0;

		unsigned int GetWidth() { return m_Width; }
		unsigned int GetHeight() { return m_Height; }

		virtual bool ShouldClose() = 0;
		virtual void Update() = 0;

		virtual void* GetNativeWindow() = 0;

		static Window* Create();
	protected:
		unsigned int m_Height;
		unsigned int m_Width;
	};
}