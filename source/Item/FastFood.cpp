#include <vector>
#include "../Game.h"
#include "../Math/Rand.h"
#include "FastFood.h"
#include "Office.h"

using namespace OT;
using namespace OT::Item;


FastFood::~FastFood()
{
	clearCustomers();
}

void FastFood::init()
{
	Item::init();

	variant = rand() % 5;
	open = false;

	sprite.SetImage(App->bitmaps["simtower/fastfood"]);
	sprite.setOrigin(0, 24);
	sprite.setPosition(getPosition().x*8, -getPosition().y*36);
	addSprite(&sprite);
	spriteNeedsUpdate = false;

	updateSprite();
}

void FastFood::encodeXML(tinyxml2::XMLPrinter & xml)
{
	Item::encodeXML(xml);
	xml.PushAttribute("variant", variant);
	xml.PushAttribute("open", open);

	// Persist arriving customers (pop to get times, then restore).
	typedef std::vector<std::pair<Customer *, double> > RestoreArriving;
	RestoreArriving v;
	while (!arrivingCustomers.empty()) {
		Customer * c = arrivingCustomers.top();
		arrivingCustomers.pop();
		v.push_back(std::make_pair(c, c->arrivalTime));
	}
	for (RestoreArriving::iterator it = v.begin(); it != v.end(); ++it) {
		xml.OpenElement("arriving");
		xml.PushAttribute("t", it->second);
		xml.CloseElement();
	}
	for (RestoreArriving::iterator it = v.begin(); it != v.end(); ++it) {
		arrivingCustomers.push(it->first);
	}

	// Persist eating customers (arrival time at fast food).
	for (std::list<Person *>::iterator it = eatingCustomers.begin(); it != eatingCustomers.end(); ++it) {
		CustomerMetadataMap::iterator m = customerMetadata.find(*it);
		if (m != customerMetadata.end()) {
			xml.OpenElement("eating");
			xml.PushAttribute("arrival", m->second.arrivalTime);
			xml.CloseElement();
		}
	}
}

void FastFood::decodeXML(tinyxml2::XMLElement & xml)
{
	Item::decodeXML(xml);
	variant = xml.IntAttribute("variant");
	open    = xml.BoolAttribute("open");

	// Restore arriving customers.
	for (tinyxml2::XMLElement * e = xml.FirstChildElement("arriving"); e; e = e->NextSiblingElement("arriving")) {
		Customer * c = new Customer(this);
		c->arrivalTime = e->DoubleAttribute("t");
		arrivingCustomers.push(c);
		customers.insert(c);
	}

	// Restore eating customers (add to people and metadata without overwriting arrival time).
	for (tinyxml2::XMLElement * e = xml.FirstChildElement("eating"); e; e = e->NextSiblingElement("eating")) {
		Customer * c = new Customer(this);
		double arr = e->DoubleAttribute("arrival");
		customers.insert(c);
		c->at = this;
		people.insert(c);
		customerMetadata[c].arrivalTime = arr;
		eatingCustomers.push_back(c);
		spriteNeedsUpdate = true;
	}
	if (!eatingCustomers.empty()) {
		game->populationNeedsUpdate = true;
	}

	updateSprite();
}

void FastFood::updateSprite()
{
	spriteNeedsUpdate = false;
	int index = 3;
	if (open) index = std::min<int>((int)ceil(people.size() / 5.0), 2);
	sprite.setTextureRect(sf::IntRect(index*128, variant*24, 128, 24));
}

void FastFood::advance(double dt)
{
	//Open
	if (game->time.checkHour(10)) {
		open = true;
		spriteNeedsUpdate = true;

		//Create new customers for today (arrivals until 19:00 so fast foods empty toward end of day).
		int today = 10;
		clearCustomers();
		for (int i = 0; i < today; i++) {
			Customer * c = new Customer(this);
			c->arrivalTime = (game->time.year - 1) * 12 + (game->time.quarter - 1) * 3 + game->time.day + Math::randd(Time::hourToAbsolute(10), Time::hourToAbsolute(19));
			customers.insert(c);
			arrivingCustomers.push(c);
		}
	}

	//Close
	if (game->time.checkHour(21) && open) {
		open = false;
		population = customerMetadata.size();
		game->populationNeedsUpdate = true;
		spriteNeedsUpdate = true;

		game->transferFunds(population * 200 - 2000, "Income from Fast Food");
	}

	//Make customers arrive.
	while (!arrivingCustomers.empty()) {
		Customer * c = arrivingCustomers.top();
		if (game->time.absolute > c->arrivalTime && !lobbyRoute.empty()) {
			arrivingCustomers.pop();
			c->journey.set(lobbyRoute);
		} else break;
	}

	//Make customers leave once they're done.
	for (std::list<Person *>::iterator ip = eatingCustomers.begin(); ip != eatingCustomers.end();) {
		Person * p = *ip;
		CustomerMetadata &m = customerMetadata[p];
		if (game->time.absolute >= m.arrivalTime + 20 * Time::kBaseSpeed || !open) {
			Route r;
			Office::Worker * worker = dynamic_cast<Office::Worker *>(p);
			if (worker && worker->office && game->items.count(worker->office))
				r = game->findRoute(this, worker->office);
			if (r.empty())
				r = game->findRoute(this, game->mainLobby);
			if (r.empty()) {
				LOG(DEBUG, "%p has no route to leave", p);
				ip++;
			} else {
				LOG(DEBUG, "%p leaving", p);
				ip = eatingCustomers.erase(ip);
				removePerson(p);
				p->journey.set(r);
			}
		} else break;
	}

	if (spriteNeedsUpdate) updateSprite();
}

void FastFood::addPerson(Person * p)
{
	Item::addPerson(p);
	CustomerMetadata & m = customerMetadata[p];
	m.arrivalTime = game->time.absolute;
	eatingCustomers.push_back(p);
	spriteNeedsUpdate = true;
}

void FastFood::removePerson(Person * p)
{
	Item::removePerson(p);
	spriteNeedsUpdate = true;
}

void FastFood::clearCustomers()
{
	for (Customers::iterator c = customers.begin(); c != customers.end(); c++)
		delete *c;
	while (!arrivingCustomers.empty()) arrivingCustomers.pop();
	eatingCustomers.clear();
	customers.clear();
	customerMetadata.clear();
}

FastFood::Customer::Customer(FastFood * item)
:	Person(item->game)
{
	arrivalTime = 0;
	Type types[] = {kMan, kWoman1, kWoman2, kWomanWithChild1};
	type = types[rand() % 4];
}

Path FastFood::getRandomBackgroundSoundPath()
{
	if (!open) return "";
	char name[128];
	snprintf(name, 128, "simtower/fastfood/%i", rand() % 3);
	// Maybe we should make the choice of the sound based on the number of customers, not
	// completely random.
	return name;
}
