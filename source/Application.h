/* Copyright (c) 2012-2015 Fabian Schuiki */
#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <functional>
#include <stack>

#include "BitmapManager.h"
#include "DataManager.h"
#include "FontManager.h"
#include "GUIManager.h"
#include "Logger.h"
#include "Path.h"
#include "State.h"
#include "SoundManager.h"

#if defined(_MSC_VER)
#include "stdint.h"
#include <direct.h>
#define mkdir(A,B)	_mkdir(A)
#elif defined(_WIN32)
#include <io.h>
#define mkdir(A,B)	mkdir(A)
#else
#include <unistd.h>
#include <errno.h>
#endif

namespace OT
{
	class Game;
	class MenuState;

	class Application
	{
		friend class State;
		friend class MenuState;

	public:
		Application(int argc, char * argv[]);

		Path getPath() const { return path; }

		Logger logger;

		sf::RenderWindow window;
		sf::VideoMode videoMode;

		DataManager   data;
		GUIManager    gui;
		GUI *         rootGUI;
		BitmapManager bitmaps;
		FontManager   fonts;
		SoundManager  sounds;

		int optionWidth;
		int optionHeight;
		bool optionFullscreen;
		float soundVolume;

		int run();

		/** Pops current state and pushes the main menu (used by Game pause menu "Quit to menu"). */
		void requestQuitToMenu();

		/** Schedules a state change to run after the current event is fully processed (avoids use-after-free when menu calls pop/push from ProcessEvent). */
		void scheduleStateChange(std::function<void()> action);

	private:
		Path path;
		void setPath(const Path & p);

		bool running;
		int exitCode;

		void init();
		void loadOptions();
		void saveOptions();
		void loop();
		void cleanup();

		std::stack<State *> states;
		void pushState(State * state);
		void popState();

		std::function<void()> pendingStateChange;

		bool dumpResources;
		Path dumpResourcesPath;
	};

	extern Application * App;
}
