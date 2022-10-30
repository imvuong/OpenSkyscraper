#pragma once
#include "../Sprite.h"
#include "Item.h"
#include <set>

namespace OT {
    namespace Item {
        class HotelSuite : public Item
        {
        public:
            OT_ITEM_CONSTRUCTOR(HotelSuite);
            OT_ITEM_PROTOTYPE(HotelSuite) {
                p->id = "suite";
                p->name = "Suite";
                p->price = 100000;
                p->size = int2(16,1);
                p->icon = 10;
            }
            virtual ~HotelSuite();

            virtual void init();

            int variant;

            Sprite sprite;
            bool spriteNeedsUpdate;
            void updateSprite();
        };
    }
}
