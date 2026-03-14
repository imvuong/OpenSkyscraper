#include "Application.h"
#include "Game.h"
#include "MenuState.h"

using namespace OT;


MenuState::MenuState(Application & app)
:	State("menu"),
	app(app),
	window(NULL)
{
}

MenuState::~MenuState()
{
	close();
}

void MenuState::activate()
{
	State::activate();
	reload();
}

void MenuState::deactivate()
{
	close();
	State::deactivate();
}

void MenuState::close()
{
	if (window) {
		window->RemoveReference();
		window->Close();
		window = NULL;
	}
}

void MenuState::reload()
{
	close();
	window = gui.loadDocument("menu.rml");
	if (window) {
		window->Show();
		Rocket::Core::Element * newGame = window->GetElementById("newgame");
		Rocket::Core::Element * load1 = window->GetElementById("load1");
		Rocket::Core::Element * load2 = window->GetElementById("load2");
		Rocket::Core::Element * load3 = window->GetElementById("load3");
		Rocket::Core::Element * quit = window->GetElementById("quit");
		if (newGame) newGame->AddEventListener("click", this);
		if (load1) load1->AddEventListener("click", this);
		if (load2) load2->AddEventListener("click", this);
		if (load3) load3->AddEventListener("click", this);
		if (quit) quit->AddEventListener("click", this);
	}
}

bool MenuState::handleEvent(sf::Event & event)
{
	return false;
}

void MenuState::advance(double dt)
{
}

void MenuState::ProcessEvent(Rocket::Core::Event & event)
{
	if (event.GetType() != "click") return;
	Rocket::Core::Element * target = event.GetTargetElement();
	while (target && target->GetId().Empty()) target = target->GetParentNode();
	if (!target) return;

	const Rocket::Core::String & id = target->GetId();
	Application * appPtr = &app;
	if (id == "newgame") {
		app.scheduleStateChange([appPtr]() {
			appPtr->popState();
			appPtr->pushState(new Game(*appPtr));
		});
	} else if (id == "load1" || id == "load2" || id == "load3") {
		int slot = (id == "load1") ? 1 : (id == "load2") ? 2 : 3;
		Game * g = new Game(app);
		if (g->loadFromFile(g->getSavePathForSlot(slot))) {
			app.scheduleStateChange([appPtr, g]() {
				appPtr->popState();
				appPtr->pushState(g);
			});
		} else {
			delete g;
		}
	} else if (id == "quit") {
		app.scheduleStateChange([appPtr]() { appPtr->popState(); });
	}
}
