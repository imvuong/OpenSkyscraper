#pragma once

#include <Rocket/Core/EventListener.h>
#include <tinyxml2.h>
#include <SFML/Audio/Sound.hpp>
#include <map>

#include "Item/Elevator/Elevator.h"
#include "Item/Factory.h"
#include "Item/Floor.h"
#include "Item/Item.h"
#include "Path.h"
#include "PathFinder/GameMap.h"
#include "PathFinder/PathFinder.h"
#include "Sky.h"
#include "Sound.h"
#include "Sprite.h"
#include "State.h"
#include "Time.h"
#include "MapWindow.h"
#include "TimeWindow.h"
#include "ToolboxWindow.h"
#include "Decorations.h"
#include "RandomEvents.h"

namespace OT {
	namespace Item { class AbstractPrototype; }

	class Game : public State, private Rocket::Core::EventListener
	{
	public:
		Application & app;
		Game(Application & app);
		virtual ~Game();

		typedef std::set<Sprite *> Sprites;
		Sprites sprites;

		unsigned int drawnSprites;

		void activate();
		void deactivate();

		bool handleEvent(sf::Event & event);
		void advance(double dt);

		Item::Factory itemFactory;

		typedef std::set<Item::Item *> ItemSet;
		ItemSet items;
		typedef std::map<int, ItemSet> ItemSetByInt;
		typedef std::map<std::string, ItemSet> ItemSetByString;
		typedef std::map<int, Item::Floor *> FloorItems;
		ItemSetByInt itemsByFloor;
		ItemSetByString itemsByType;
		FloorItems floorItems;
		Item::Item * mainLobby;
		Item::Item * metroStation;
		void addItem(Item::Item * item);
		void removeItem(Item::Item * item);
		void extendFloor(int floor, int minX, int maxX);

		int funds;
		int rating;
		int population;
		bool populationNeedsUpdate;
		/** Set when a VIP visit ends with satisfaction; required for 4-star rating. */
		bool favorableVipVisit;
		void transferFunds(int f, std::string message = std::string());
		void setFunds(int f);
		void setRating(int r);
		void setPopulation(int p);
		void ratingMayIncrease();
		void openTerroristEventDialog();

		MapWindow     mapWindow;
		ToolboxWindow toolboxWindow;
		TimeWindow    timeWindow;

		Time time;
		int speedMode;
		void setSpeedMode(int sm);

		std::string selectedTool;
		int2 toolPosition;
		Item::AbstractPrototype * toolPrototype;
		Item::Item * itemBelowCursor;
		void selectTool(const char * tool);

		Sky sky;

		Item::Elevator::Elevator * draggingElevator;
		int draggingElevatorStart;
		bool draggingElevatorLower;
		int draggingMotor;

		// Sound cockSound;
		sf::Sound *morningSound;
		// Sound bellsSound;
		// Sound eveningSound;

		typedef std::set<Sound *> SoundSet;
		SoundSet autoreleaseSounds;
		SoundSet playingSounds;
		std::map<Path, double> soundPlayTimes;
		void playOnce(Path sound);
		void playRandomBackgroundSound();

		void updateRoutes();
		Route findRoute(Item::Item * start, Item::Item * destination, bool serviceRoute = false);

		/** Returns the default save path (saves/default.tower under first data dir). Creates saves dir if needed. */
		Path getSavePath() const;
		/** Returns save path for slot 1..3 (slot 1 = default.tower, 2 = slot2.tower, 3 = slot3.tower). */
		Path getSavePathForSlot(int slot) const;
		/** Clears all items and loads tower from the given path. Returns true on success. */
		bool loadFromFile(Path path);

		/** Highest above-ground floor (1..100) that has any built content; 0 if none. */
		int getTowerTopFloor() const;
		/** True if every floor from 1 to getTowerTopFloor() has at least one floor tile (for Tower rating). */
		bool isTowerFullyDeveloped() const;

		Route visualizeRoute;

		RandomEvents randomEvents;

		GameMap gameMap;
		PathFinder pathFinder;
		Decorations decorations;

	private:
		double zoom;
		double2 poi;

		void reloadGUI();

		void encodeXML(tinyxml2::XMLPrinter & xml);
		void decodeXML(tinyxml2::XMLDocument & xml);

		/** Saves the current tower to the given path. Returns true on success. */
		bool saveToFile(Path path);
		/** Removes and deletes all items; resets mainLobby and metroStation. */
		void clearState();

		bool pauseMenuOpen;
		Rocket::Core::ElementDocument * pauseMenuWindow;
		Rocket::Core::ElementDocument * pauseQuitConfirmWindow;
		int pauseMenuPrevSpeedMode;
		Rocket::Core::ElementDocument * terroristEventWindow;
		int terroristEventPrevSpeedMode;
		bool dirty;
		void showPauseMenu();
		void hidePauseMenu();
		void ProcessEvent(Rocket::Core::Event & event);
	};
}
