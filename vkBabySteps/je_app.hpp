#pragma once

#include <SDL3/SDL.h>

#include <memory>

#include "je_window.hpp"
#include "je_instance.hpp"

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
		// JeInstance& instance() { return *jeInstance; };

	private:
		std::unique_ptr<JeWindow> jeWindow;
		std::unique_ptr<JeInstance> jeInstance;

		bool quit = false;

		void initSDL() const;
	};
}