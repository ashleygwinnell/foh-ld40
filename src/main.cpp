
#include <ARK2D/Core/GameContainer.h>
#include <ARK2D/Tests/GamepadsTest.h>
#include "DefaultGame.h"

int main(int argc, char** argv) {

	//ARK2D::getLog()->setFilter(ARK::Core::Log::TYPE_INFORMATION);
	ARK2D::getLog()->setFilter(ARK::Core::Log::TYPE_VERBOSE);
	//ARK::Tests::NetTest::start();
	//ARK::Tests::GamepadsTest::start();
	//ARK::Tests::ShaderTest::start();
	//return 0;

	DefaultGame* game = new DefaultGame("Carl Hates Christmas");
	GameContainer* container = new GameContainer(*game, 284, 160, 32, false);
	container->setShowingFPS(false);
	container->setClearColor(Color::black);
	container->setIcon("data/icon.ico");
	container->setCursorVisible(true);
	container->start();

	return 0;
}

