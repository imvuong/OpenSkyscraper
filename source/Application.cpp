#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>

#include "Application.h"
#include "Game.h"
#include "SimTowerLoader.h"

using namespace OT;
using namespace std;

Application * OT::App = NULL;

Application::Application(int argc, char * argv[])
:	data(this),
	fonts(this),
	bitmaps(this)
{
	assert(App == NULL && "Application initialized multiple times");
	App = this;
	
	assert(argc >= 1 && "argv[0] is required");
	path = Path(argv[0]);
#ifdef __APPLE__
	path = Path("../MacOS").down(path.name());
#endif
	
	//Special debug defaults.
#ifdef BUILD_DEBUG
	logger.setLevel(Logger::DEBUG);
	char logname[128];
	snprintf(logname, 128, "debug-%li.log", (long int)time(NULL));
	logger.setOutputPath(/*dir.down(*/logname/*)*/);
#endif
	
	//Parse command line arguments.
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--debug") == 0) {
			logger.setLevel(Logger::DEBUG);
		}
		if (strcmp(argv[i], "--log") == 0) {
			assert(i+1 < argc && "--log is missing path");
			logger.setOutputPath(argv[i++]);
		}
	}
	
	LOG(DEBUG,
		"constructed\n"
		"    path     = %s",
		path.str().c_str()
	);
	LOG(IMPORTANT, "ready");
}

/** Runs the application. */
int Application::run()
{
	running = true;
	exitCode = 0;
	
	if (exitCode == 0) init();
	if (exitCode == 0) loop();
	if (exitCode == 0) cleanup();
	
	running = false;
	
	if (exitCode < 0) {
		LOG(ERROR, "exitCode = %i", exitCode);
	} else {
		LOG(INFO,  "exitCode = %i", exitCode);
	}
	
	return exitCode;
}

void Application::init()
{
	data.init();
	
	/*WindowsNEExecutable exe;
	DataManager::Paths paths = data.paths("SIMTOWER.EXE");
	bool success;
	for (int i = 0; i < paths.size() && !success; i++) {
		success = exe.load(paths[i]);
	}
	if (!success) {
		LOG(WARNING, "unable to load SimTower executable");
	}
	//TODO: make this dependent on a command line switch --dump-simtower <path>.
	exe.dump("~/SimTower Raw");*/
	
	SimTowerLoader * simtower = new SimTowerLoader(this);
	if (!simtower->load()) {
		LOG(WARNING, "unable to load SimTower resources");
	}
	//TODO: make this dependent on a command line switch
	//simtower->dump("~/SimTower Resources");
	delete simtower; simtower = NULL;
	//exitCode = 1;
	
	videoMode.Width        = 1280;
	videoMode.Height       = 768;
	videoMode.BitsPerPixel = 32;
	
	window.Create(videoMode, "OpenSkyscraper SFML");
	
	if (!gui.init(&window)) {
		LOG(ERROR, "unable to initialize gui");
		exitCode = -1;
		return;
	}
	
	//DEBUG: load some GUI
	Path rocket = data.paths("debug/rocket").front();
	Rocket::Core::FontDatabase::LoadFontFace(rocket.down("Delicious-Bold.otf").c_str());
	Rocket::Core::FontDatabase::LoadFontFace(rocket.down("Delicious-BoldItalic.otf").c_str());
	Rocket::Core::FontDatabase::LoadFontFace(rocket.down("Delicious-Italic.otf").c_str());
	Rocket::Core::FontDatabase::LoadFontFace(rocket.down("Delicious-Roman.otf").c_str());
	
	Game * game = new Game(*this);
	pushState(game);
	
	/*Rocket::Core::ElementDocument * cursor = gui.context->LoadMouseCursor(rocket.down("cursor.rml").c_str());
	if (cursor) {
		cursor->RemoveReference();
	}*/
	
	/*Rocket::Core::ElementDocument * document = gui.context->LoadDocument(rocket.down("demo.rml").c_str());
	if (document) {
		LOG(DEBUG, "loaded demo.rml");
		document->Show();
		document->RemoveReference();
	}*/
}

void Application::loop()
{
	sf::Clock clock;
	sf::String rateIndicator("<not available>", fonts["UbuntuMono-Regular.ttf"], 16);
	double rateIndicatorTimer = 0;
	double rateDamped = 0;
	double rateDampFactor = 0;
	double dt_max = 0, dt_min = 0;
	int dt_maxmin_resetTimer = 0;
	
	while (window.IsOpened() && exitCode == 0 && !states.empty()) {
		double dt_real = clock.GetElapsedTime();
		//dt_max = (dt_max + dt_real * dt_real * 0.5) / (1 + dt_real * 0.5);
		//dt_min = (dt_min + dt_real * dt_real * 0.5) / (1 + dt_real * 0.5);
		if (dt_real > dt_max) {
			dt_max = dt_real;
			dt_maxmin_resetTimer = 0;
		}
		if (dt_real < dt_min) {
			dt_min = dt_real;
			dt_maxmin_resetTimer = 0;
		}
		double dt = std::max<double>(dt_real, 0.1); //avoids FPS dropping below 10 Hz
		clock.Reset();
		
		//Update the rate indicator.
		rateDampFactor = (dt_real * 1);
		rateDamped = (rateDamped + dt_real * rateDampFactor) / (1 + rateDampFactor);
		if ((rateIndicatorTimer += dt_real) > 0.5) {
			rateIndicatorTimer -= 0.5;
			if (++dt_maxmin_resetTimer >= 2*3) {
				dt_maxmin_resetTimer = 0;
				dt_max = dt_real;
				dt_min = dt_real;
			}
		}
		
		//Handle events.
		sf::Event event;
		while (window.GetEvent(event)) {
			if (event.Type == sf::Event::Resized) {
				LOG(INFO, "resized (%i, %i)", window.GetWidth(), window.GetHeight());
				window.GetDefaultView().SetFromRect(sf::FloatRect(0, 0, window.GetWidth(), window.GetHeight()));
			}
			if (event.Type == sf::Event::KeyPressed && event.Key.Code == sf::Key::Escape) {
				exitCode = 1;
				continue;
			}
			if (!states.empty()) {
				if (states.top()->handleEvent(event))
					continue;
			}
			if (event.Type == sf::Event::Closed) {
				LOG(WARNING, "current state did not handle sf::Event::Closed");
				exitCode = 1;
				continue;
			}
			gui.handleEvent(event);
		}
		
		//Make the current state do its work.
		if (!states.empty()) {
			states.top()->advance(dt);
		}
		
		//Draw the GUI.
		gui.draw();
		
		//Draw the debugging overlays.
		char dbg[1024];
		snprintf(dbg, 32, "%.0f Hz [%.0f..%.0f]", 1.0/rateDamped, 1.0/dt_max, 1.0/dt_min);
		if (!states.empty()) {
			strcat(dbg, "\n");
			strcat(dbg, states.top()->debugString);
		}
		rateIndicator.SetText(dbg);
		
		window.SetView(window.GetDefaultView());
		sf::FloatRect r = rateIndicator.GetRect();
		sf::Shape bg = sf::Shape::Rectangle(r.Left, r.Top, r.Right, r.Bottom, sf::Color::Black);
		window.Draw(bg);
		window.Draw(rateIndicator);
		
		//Swap buffers.
		window.Display();
	}
}

void Application::cleanup()
{
	while (!states.empty()) {
		popState();
	}
	
	window.Close();
}

/** Pushes the given State ontop of the state stack, causing it to receive events. */
void Application::pushState(State * state)
{
	assert(state != NULL && "pushState() requires non-NULL state");
	LOG(INFO, "pushing state %s", typeid(*state).name());
	if (!states.empty()) {
		states.top()->deactivate();
	}
	states.push(state);
	state->activate();
}

/** Pops the topmost State off the state stack, causing the underlying state to regain control.
 *  The application will terminate once there are no more states on the stack. */
void Application::popState()
{
	assert(!states.empty() && "popState() requires at least one state on the states stack");
	LOG(INFO, "popping state %s", typeid(*states.top()).name());
	states.top()->deactivate();
	states.pop();
	if (!states.empty()) {
		states.top()->activate();
	}
}
