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
            }
            virtual ~HotelSingle();

            virtual void init();

            int variant;

            Sprite sprite;
            bool spriteNeedsUpdate;
            void updateSprite();
        };
    }
}
