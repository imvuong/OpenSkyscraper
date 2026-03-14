#pragma once
#include "../Sprite.h"
#include "Item.h"
#include <set>

namespace OT {
    namespace Item {
        class Shop : public Item
        {
        public:
            OT_ITEM_CONSTRUCTOR(Shop);
            OT_ITEM_PROTOTYPE(Shop) {
                p->id = "shop";
                p->name = "Shop";
                p->price = 100000;
                p->size = int2(16,1);
                p->icon = 13;
                p->minRating = 2;
            }
            virtual ~Shop();

            virtual void init();

            int variant;

            Sprite sprite;
            bool spriteNeedsUpdate;
            void updateSprite();
        };
    }
}
