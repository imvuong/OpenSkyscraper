#pragma once

#include <Rocket/Core/ElementDocument.h>
#include <Rocket/Core/EventListener.h>

#include "State.h"

namespace OT {

	class Application;
	class Game;

	class MenuState : public State, private Rocket::Core::EventListener
	{
	public:
		MenuState(Application & app);
		virtual ~MenuState();

		void activate();
		void deactivate();
		bool handleEvent(sf::Event & event);
		void advance(double dt);

		virtual void ProcessEvent(Rocket::Core::Event & event);

	private:
		Application & app;
		Rocket::Core::ElementDocument * window;

		void reload();
		void close();
	};
}
