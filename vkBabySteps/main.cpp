#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <stdexcept>
#include <iostream>

#include "je_app.hpp"
#include "je_window.hpp"

constexpr int WND_HEIGHT = 600;
constexpr int WND_WIDTH = 800;

int main(int, char**)
{
	je::JeApp jeApp{"vkBabySteps", WND_WIDTH, WND_HEIGHT};

	try
	{
		jeApp.run();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << "\n";
		return 1;
	}

	return 0;
}