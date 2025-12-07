#pragma once

#include <SDL3/SDL.h>

#include <memory>

#include "je_window.hpp"
#include "je_instance.hpp"
#include "je_device.hpp"

namespace je
{
	class JeApp
	{
	public:
		JeApp(const char* appTitle, uint32_t w, uint32_t h);
		~JeApp();

		JeApp(const JeApp&) = delete;
		JeApp& operator=(const JeApp&) = delete;

		void run();

		JeWindow& window() { return *jeWindow; };
		const JeWindow& window() const { return *jeWindow; };

	private:
		std::unique_ptr<JeWindow> jeWindow;
		std::unique_ptr<JeInstance> jeInstance;
		std::unique_ptr<JeDevice> jeDevice;

		bool quit = false;

		void initSDL() const;
	};
}