#include <cassert>
#include "Game.h"
#include "Item/Item.h"
#include "Person.h"

using namespace OT;


Person::Person(Game * game, Type type)
:	GameObject(game),
	journey(this),
	type(type)
{
	at     = NULL;
	stress = 0.0;
}

Person::~Person()
{
	if (at) {
		LOG(DEBUG, "forcefully removed from %s", at->desc().c_str());
		at->removePerson(this);
	}
}

Person::Journey::Journey(Person * p)
:	person(p)
{
	fromFloor = 0;
	item = NULL;
	toFloor = 0;
}

void Person::Journey::set(const Route & r)
{
	while (!nodes.empty()) nodes.pop();
	for (std::vector<Route::Node>::const_iterator nit = r.nodes.begin(); nit != r.nodes.end(); nit++) {
		nodes.push(*nit);
	}
	toFloor = nodes.front().toFloor;
	next();
}

void Person::Journey::next()
{
	//Peek next node (we will pop current first).
	assert(!nodes.empty());
	nodes.pop();
	if (nodes.empty()) return;
	Route::Node nextNode = Route::Node();
	nextNode.item = nodes.front().item;
	nextNode.toFloor = nodes.front().toFloor;

	//If the next node's item was removed (e.g. elevator deleted), invalidate route and stay put.
	if (!nextNode.item || person->game->items.count(nextNode.item) == 0) {
		while (!nodes.empty()) nodes.pop();
		item = NULL;
		toFloor = fromFloor;
		return;
	}

	//Remove the person from where he/she is currently at.
	if (person->at) person->at->removePerson(person);

	//Keep the current floor around.
	fromFloor = toFloor;

	//Add the person to the next node's item.
	item    = nextNode.item;
	toFloor = nextNode.toFloor;
	item->addPerson(person);
}

int Person::getWidth()
{
	return (type >= kHousekeeper ? 16 : 8);
}
