#pragma once
#include "../Sprite.h"
#include "Item.h"
#include <set>

namespace OT {
    namespace Item {
        class HotelSingle : public Item
        {
        public:
            OT_ITEM_CONSTRUCTOR(HotelSingle);
            OT_ITEM_PROTOTYPE(HotelSingle) {
                p->id = "single";
                p->name = "Single";
                p->price = 100000;
                p->size = int2(16,1);
                p->icon = 8;
                p->minRating = 1;
            }
            virtual ~HotelSingle();

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
