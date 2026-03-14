#pragma once
#include "../Sprite.h"
#include "Item.h"
#include <set>

namespace OT {
    namespace Item {
        class HotelDouble : public Item
        {
        public:
            OT_ITEM_CONSTRUCTOR(HotelDouble);
            OT_ITEM_PROTOTYPE(HotelDouble) {
                p->id = "double";
                p->name = "Double";
                p->price = 100000;
                p->size = int2(16,1);
                p->icon = 9;
                p->minRating = 2;
            }
            virtual ~HotelDouble();

            virtual void init();
            virtual void advance(double dt) override;
            virtual void encodeXML(tinyxml2::XMLPrinter & xml) override;
            virtual void decodeXML(tinyxml2::XMLElement & xml) override;

            enum State { kEmpty = 0, kOccupied = 1, kDirty = 2 };
            State state;
            double dirtySince;

            int variant;

            Sprite sprite;
            bool spriteNeedsUpdate;
            void updateSprite();
        };
    }
}
