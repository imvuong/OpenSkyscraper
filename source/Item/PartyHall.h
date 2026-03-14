#pragma once
#include "../Sprite.h"
#include "Item.h"
#include <queue>
#include <deque>

namespace OT {
	namespace Item {
		class PartyHall : public Item
		{
		public:
			OT_ITEM_CONSTRUCTOR(PartyHall);
			OT_ITEM_PROTOTYPE(PartyHall) {
				p->id    = "partyhall";
				p->name  = "Party Hall";
				p->price = 500000;
				p->size  = int2(24,2);
				p->icon  = 15;
				p->minRating = 2;
			}
			virtual ~PartyHall();
			
			virtual void init();
			
			virtual void encodeXML(tinyxml2::XMLPrinter & xml);
			virtual void decodeXML(tinyxml2::XMLElement & xml);
			
			bool open;

			class Customer : public Person {
			public:
				Customer(PartyHall * item);
				double arrivalTime;
				struct laterThan : public std::binary_function<Customer *, Customer *, bool> {
					bool operator() (const Customer * a, const Customer * b) const {
						return a->arrivalTime > b->arrivalTime;
					}
				};
			};
			typedef std::set<Customer *> Customers;
			Customers customers;
			std::priority_queue<Customer *, std::deque<Customer *>, Customer::laterThan> arrivingCustomers;
			void clearCustomers();
			
			Sprite sprite;
			bool spriteNeedsUpdate;
			void updateSprite();
			
			virtual void advance(double dt);

			virtual void addPerson(Person * p);
			virtual void removePerson(Person * p);

			Path getRandomBackgroundSoundPath();
		};
	}
}
