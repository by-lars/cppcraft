#pragma once
#include "Core/GLWindow.h"

namespace ZuneCraft {
	class ZuneWindow : public Window {
	public:
		ZuneWindow();
		~ZuneWindow();
		
		bool ShouldClose() override;
		void Update() override;
		void* GetNativeWindow() override;
	};
}