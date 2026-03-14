#include "../Game.h"
#include "../Math/Rand.h"
#include "Cinema.h"

using namespace OT;
using namespace OT::Item;

namespace {
	const int kRevenuePerCustomer = 500;
	const int kOperatingCostPerScreening = 2000;
}

Cinema::~Cinema()
{
}

void Cinema::init()
{
	Item::init();

	open = false;
	playing = false;
	movieType = rand() % 15;
	animation = 0;
	animationFrame = 0;

	hallSprite.SetImage(App->bitmaps["simtower/cinema/hall"]);
	hallSprite.setOrigin(-56, 60);
	screenSprite.SetImage(App->bitmaps["simtower/cinema/screens"]);
	screenSprite.setOrigin(0, 60);
	addSprite(&hallSprite);
	addSprite(&screenSprite);
	spriteNeedsUpdate = false;

	updateSprite();
}

void Cinema::encodeXML(tinyxml2::XMLPrinter & xml)
{
	Item::encodeXML(xml);
	xml.PushAttribute("open", open);
	xml.PushAttribute("playing", playing);
	xml.PushAttribute("movie", movieType);
}

void Cinema::decodeXML(tinyxml2::XMLElement & xml)
{
	Item::decodeXML(xml);
	open = xml.BoolAttribute("open");
	playing = xml.BoolAttribute("playing");
	movieType = xml.IntAttribute("movie");
	updateSprite();
}

void Cinema::updateSprite()
{
	spriteNeedsUpdate = false;
	int hallIndex = 0;
	int screenIndex = 0;
	if (open) {
		if (playing) {
			hallIndex = 3 + animationFrame;
			screenIndex = 3 + movieType;
		} else {
			// Hall sprite reflects occupancy: empty (1) vs has guests (2). More granular frames could use people.size() thresholds.
			const int kHallEmpty = 1, kHallWithGuests = 2;
			hallIndex = (people.size() > 0 ? kHallWithGuests : kHallEmpty);
			screenIndex = hallIndex;
		}
	}
	hallSprite.setTextureRect(sf::IntRect(hallIndex*192, 0, 192, 60));
	// hallSprite.resize(192, 60);
	screenSprite.setTextureRect(sf::IntRect(screenIndex*56, 0, 56, 60));
	// screenSprite.resize(56, 60);
	hallSprite.setPosition(getPositionPixels().x, -getPositionPixels().y);
	screenSprite.setPosition(getPositionPixels().x, -getPositionPixels().y);
}

void Cinema::advance(double dt)
{
	//Open
	if (game->time.checkHour(13) || game->time.checkHour(19)) {
		open = true;
		playing = false;
		spriteNeedsUpdate = true;

		//Fill in the customers for this screening.
		clearCustomers();
		const int numCustomers = 50;
		for (int i = 0; i < numCustomers; i++) {
			Customer *p = new Customer(this);
			customers.insert(p);

			//Make the customer journey to the cinema immediately.
			p->journey.set(lobbyRoute);
		}
	}

	//Start Screening
	if ((game->time.checkHour(15) || game->time.checkHour(21)) && open) {
		playing = true;
		spriteNeedsUpdate = true;
		game->timeWindow.showMessage("Movie starts at the Movie Theatre");
	}

	//Close
	if ((game->time.checkHour(17) || game->time.checkHour(23)) && open) {
		open = false;
		playing = false;
		spriteNeedsUpdate = true;

		// Revenue when show completes: per-customer ticket income minus operating cost.
		game->transferFunds((int)customers.size() * kRevenuePerCustomer - kOperatingCostPerScreening, "Income from Movie Theatre");

		//Make the customers leave.
		const Route &r = game->findRoute(this, game->mainLobby);
		for (Customers::iterator c = customers.begin(); c != customers.end(); c++) {
			Customer *p = *c;
			if (r.empty()) {
				LOG(DEBUG, "%p has no route to leave", p);
			} else {
				LOG(DEBUG, "%p leaving", p);
				removePerson(p);
				p->journey.set(r);
			}
		}
	}

	//Animate the sprite.
	animation = fmod(animation + dt, 1);
	int af = floor(animation * 2);
	if (af != animationFrame) {
		animationFrame = af;
		spriteNeedsUpdate = true;
	}

	if (spriteNeedsUpdate) updateSprite();
}

Path Cinema::getRandomBackgroundSoundPath()
{
	if (!open || !playing) return "";
	char name[128];
	snprintf(name, 128, "simtower/cinema/movie%i", movieType);
	return name;
}

void Cinema::addPerson(Person * p)
{
	Item::addPerson(p);
	spriteNeedsUpdate = true;
}

void Cinema::removePerson(Person * p)
{
	Item::removePerson(p);
	spriteNeedsUpdate = true;
}

Cinema::Customer::Customer(Cinema * item)
:	Person(item->game)
{
	Type types[] = {kMan, kWoman1, kWoman2, kWomanWithChild1};
	type = types[rand() % 4];
}

/** Removes all customers from the item. */
void Cinema::clearCustomers()
{
	for (Customers::iterator c = customers.begin(); c != customers.end(); c++)
		delete *c;
	customers.clear();
}
