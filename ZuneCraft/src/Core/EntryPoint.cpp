#include "Core/Game.h"
#include "Core/Base.h"
#include "Utility/File.h"

int main(int argc, char* argv[]) {
	ZuneCraft::Logger::Init();

	ZuneCraft::Game* game = new ZuneCraft::Game();

	while (game->GetWindow().ShouldClose() == false) {
		game->Update();
	}

	delete game;

	ZuneCraft::Logger::Shutdown();

	return 0;
}
