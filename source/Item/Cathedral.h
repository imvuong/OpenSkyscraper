/* Copyright (c) 2012-2015 Fabian Schuiki */
#pragma once
#include "../Sprite.h"
#include "Item.h"

namespace OT {
	namespace Item {
		class Cathedral : public Item
		{
		public:
			OT_ITEM_CONSTRUCTOR(Cathedral);
			OT_ITEM_PROTOTYPE(Cathedral) {
				p->id    = "cathedral";
				p->name  = "Cathedral";
				p->price = 5000000;
				p->size  = int2(16, 2);
				p->icon  = 20;
				p->minRating = 4;
			}
			virtual ~Cathedral();

			virtual void init();

			virtual void encodeXML(tinyxml2::XMLPrinter & xml);
			virtual void decodeXML(tinyxml2::XMLElement & xml);

			Sprite sprite;
			void updateSprite();
		};
	}
}
