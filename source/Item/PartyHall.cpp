#include <vector>
#include "../Game.h"
#include "../Math/Rand.h"
#include "PartyHall.h"

using namespace OT;
using namespace OT::Item;


PartyHall::~PartyHall()
{
	clearCustomers();
}

void PartyHall::init()
{
	Item::init();

	open = false;

	sprite.SetImage(App->bitmaps["simtower/partyhall"]);
	sprite.setOrigin(0, 60);
	addSprite(&sprite);
	spriteNeedsUpdate = false;

	updateSprite();
}

void PartyHall::encodeXML(tinyxml2::XMLPrinter & xml)
{
	Item::encodeXML(xml);
	xml.PushAttribute("open", open);
}

void PartyHall::decodeXML(tinyxml2::XMLElement & xml)
{
	Item::decodeXML(xml);
	open = xml.BoolAttribute("open");
	updateSprite();
}

void PartyHall::updateSprite()
{
	spriteNeedsUpdate = false;
	int index = (open ? 1 : 0);
	sprite.setTextureRect(sf::IntRect(index*192, 0, 192, 60));
	sprite.setPosition(getPositionPixels().x, -getPositionPixels().y);
}

PartyHall::Customer::Customer(PartyHall * item)
:	Person(item->game)
{
	Type types[] = { Person::kMan, Person::kWoman1, Person::kWoman2, Person::kWomanWithChild1 };
	type = types[rand() % 4];
}

void PartyHall::clearCustomers()
{
	while (!arrivingCustomers.empty()) arrivingCustomers.pop();
	for (Customers::iterator c = customers.begin(); c != customers.end(); c++)
		delete *c;
	customers.clear();
}

void PartyHall::addPerson(Person * p)
{
	Item::addPerson(p);
	spriteNeedsUpdate = true;
}

void PartyHall::removePerson(Person * p)
{
	Item::removePerson(p);
	spriteNeedsUpdate = true;
}

void PartyHall::advance(double dt)
{
	// Open: two parties per day (13–17 and 19–23)
	if (game->time.checkHour(13) || game->time.checkHour(19)) {
		open = true;
		spriteNeedsUpdate = true;
		clearCustomers();
		const int numGuests = 30 + (rand() % 31);
		const double partyStart = game->time.absolute;
		const double arrivalEnd = partyStart + Time::hourToAbsolute(1);
		for (int i = 0; i < numGuests; i++) {
			Customer * c = new Customer(this);
			c->arrivalTime = partyStart + Math::randd(0, arrivalEnd - partyStart);
			customers.insert(c);
			arrivingCustomers.push(c);
		}
	}

	// Close: income by visitor count, then send everyone to lobby
	if ((game->time.checkHour(17) || game->time.checkHour(23)) && open) {
		open = false;
		spriteNeedsUpdate = true;
		const int visitors = (int)people.size();
		game->transferFunds(visitors * 350 - 2000, "Income from Party Hall");
		const Route & r = game->findRoute(this, game->mainLobby);
		std::vector<Person *> toLeave(people.begin(), people.end());
		for (std::vector<Person *>::iterator it = toLeave.begin(); it != toLeave.end(); it++) {
			Person * p = *it;
			if (r.empty()) {
				LOG(DEBUG, "PartyHall: no route to lobby for %p", p);
			} else {
				removePerson(p);
				p->journey.set(r);
			}
		}
	}

	// Staggered arrival: send guests to the hall when their arrival time is due
	while (!arrivingCustomers.empty() && !lobbyRoute.empty()) {
		Customer * c = arrivingCustomers.top();
		if (game->time.absolute < c->arrivalTime) break;
		arrivingCustomers.pop();
		c->journey.set(lobbyRoute);
	}

	if (spriteNeedsUpdate) updateSprite();
}

Path PartyHall::getRandomBackgroundSoundPath()
{
	if (!open) return "";
	return "simtower/partyhall";
}
