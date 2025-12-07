#include "je_app.hpp"

#include <stdexcept>

namespace je
{
	JeApp::JeApp(const char* appTitle, uint32_t w, uint32_t h)
	{
		initSDL();

		jeWindow = std::make_unique<JeWindow>(appTitle, w, h);
		jeInstance = std::make_unique<JeInstance>("vkBabySteps", jeWindow->get());

		jeWindow->createSurface(jeInstance->get());

		jeDevice = std::make_unique<JeDevice>(jeInstance.get()->get(), jeWindow->getSurface());
	}

	JeApp::~JeApp()
	{
		SDL_Vulkan_UnloadLibrary();
		SDL_Quit();
	}

	void JeApp::run()
	{
		SDL_Event e;

		while (!quit)
		{
			while (SDL_PollEvent(&e))
			{
				if (e.type == SDL_EVENT_QUIT)
					quit = true;
			}
		}
	}

	void JeApp::initSDL() const
	{
		if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
			throw std::runtime_error("Failed to initialize SDL");

		if (!SDL_Vulkan_LoadLibrary(0))
			throw std::runtime_error("SDL failed to load vulkan");
	}
}