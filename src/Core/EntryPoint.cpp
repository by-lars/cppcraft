#include "Core/Game.h"
#include "Core/Base.h"

#ifdef ZC_PLATFORM_ZUNE
extern "C" DWORD SetSystemMemoryDivision (DWORD dwStorePages);
#endif

int main(int argc, char* argv[]) {
#ifdef ZC_PLATFORM_ZUNE
	SetSystemMemoryDivision(4500);
#endif

	ZuneCraft::Logger::Init();
	ZuneCraft::Game* game = new ZuneCraft::Game();

	while (game->GetWindow().ShouldClose() == false) {
		game->Update();
	}

	delete game;

	ZuneCraft::Logger::Shutdown();

	return 0;
}
