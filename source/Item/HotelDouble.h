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
            }
            virtual ~HotelDouble();

            virtual void init();

            int variant;

            Sprite sprite;
            bool spriteNeedsUpdate;
            void updateSprite();
        };
    }
}
