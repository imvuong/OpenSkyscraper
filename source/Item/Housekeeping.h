#pragma once
#include "../Sprite.h"
#include "Item.h"
#include <set>

namespace OT {
    namespace Item {
        class Housekeeping : public Item
        {
        public:
            OT_ITEM_CONSTRUCTOR(Housekeeping);
            OT_ITEM_PROTOTYPE(Housekeeping) {
                p->id = "housekeeping";
                p->name = "Housekeeping";
                p->price = 100000;
                p->size = int2(16,1);
                p->icon = 23;
                p->minRating = 1;
            }
            virtual ~Housekeeping();

            virtual void init();

            int variant;

            Sprite sprite;
            bool spriteNeedsUpdate;
            void updateSprite();
        };
    }
}
