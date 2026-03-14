#include <vector>
#include "../Game.h"
#include "../Math/Rand.h"
#include "../Person.h"
#include "Metro.h"
#include "FastFood.h"
#include "Restaurant.h"
#include "Cinema.h"
#include "PartyHall.h"
#include "Factory.h"

using namespace OT;
using namespace OT::Item;


Metro::~Metro()
{
	// Remove and delete anyone still at the station (e.g. waiting for train)
	People toRemove(people.begin(), people.end());
	for (People::iterator it = toRemove.begin(); it != toRemove.end(); it++) {
		Person * p = *it;
		removePerson(p);
		delete p;
	}
}

void Metro::init()
{
	Item::init();

	open = true;
	trainPresent = true;
	wasTrainPresent = true;

	station.SetImage(App->bitmaps["simtower/metro/station"]);
	station.setOrigin(0, 96);
	platform.SetImage(App->bitmaps["simtower/metro/station"]);
	platform.setOrigin(0, 30);
	addSprite(&station);
	addSprite(&platform);
	spriteNeedsUpdate = true;

	assert(game->metroStation == NULL);
	game->metroStation = this;

	updateSprite();
}

void Metro::encodeXML(tinyxml2::XMLPrinter & xml)
{
	Item::encodeXML(xml);
	xml.PushAttribute("open", open);
	xml.PushAttribute("trainPresent", trainPresent);
}

void Metro::decodeXML(tinyxml2::XMLElement & xml)
{
	Item::decodeXML(xml);
	open = xml.BoolAttribute("open");
	trainPresent = xml.BoolAttribute("trainPresent");
	wasTrainPresent = trainPresent;
	updateSprite();
}

void Metro::updateSprite()
{
	spriteNeedsUpdate = false;
	int stationIndex = 2, platformIndex = 2;
	if (open) {
		stationIndex = 1;
		platformIndex = (trainPresent ? 0 : 1);
	}

	station.setTextureRect(sf::IntRect(stationIndex*240, 0, 240, 66));
	platform.setTextureRect(sf::IntRect(platformIndex*240, 66, 240, 30));
	station.setPosition(getPositionPixels().x, -getPositionPixels().y);
	platform.setPosition(getPositionPixels().x, -getPositionPixels().y);
}

void Metro::advance(double dt)
{
	// Open 7–23
	if (game->time.checkHour(7)) {
		open = true;
		spriteNeedsUpdate = true;
	}
	if (game->time.checkHour(23) && open) {
		open = false;
		spriteNeedsUpdate = true;
	}

	// Train on fixed schedule: present for one hour at 7–8, 9–10, …, 21–22
	int h = (int)game->time.hour;
	bool wantTrain = (h >= 7 && h < 23 && (h % 2) == 1);
	if (wantTrain != trainPresent) {
		trainPresent = wantTrain;
		spriteNeedsUpdate = true;

		if (trainPresent) {
			// Train arrives: spawn customers for underground shops
			std::vector<Item *> undergroundShops;
			const char * types[] = { "fastfood", "restaurant", "cinema", "partyhall" };
			for (int t = 0; t < 4; t++) {
				Game::ItemSetByString::const_iterator it = game->itemsByType.find(types[t]);
				if (it == game->itemsByType.end()) continue;
				for (Game::ItemSet::const_iterator i = it->second.begin(); i != it->second.end(); i++) {
					if ((*i)->position.y < 0) undergroundShops.push_back(*i);
				}
			}
			if (!undergroundShops.empty()) {
				const int numPassengers = 15 + (rand() % 16);
				Person::Type personTypes[] = { Person::kMan, Person::kWoman1, Person::kWoman2, Person::kWomanWithChild1 };
				for (int i = 0; i < numPassengers; i++) {
					Item * dest = undergroundShops[rand() % undergroundShops.size()];
					Route r = game->findRoute(this, dest);
					if (r.empty()) continue;
					Person * p = new Person(game, personTypes[rand() % 4]);
					p->journey.set(r);
				}
			}
		} else {
			// Train departs: everyone at the station boards and leaves
			std::vector<Person *> toRemove(people.begin(), people.end());
			for (std::vector<Person *>::iterator it = toRemove.begin(); it != toRemove.end(); it++) {
				Person * p = *it;
				removePerson(p);
				delete p;
			}
		}
	}
	wasTrainPresent = trainPresent;

	if (spriteNeedsUpdate) updateSprite();
}
