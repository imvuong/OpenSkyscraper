#pragma once
#include "../Sprite.h"
#include "Item.h"
#include <set>

namespace OT {
    namespace Item {
        class ParkingSpace : public Item
        {
        public:
            OT_ITEM_CONSTRUCTOR(ParkingSpace);
            OT_ITEM_PROTOTYPE(ParkingSpace) {
                p->id = "space";
                p->name = "Parking Space";
                p->price = 100000;
                p->size = int2(16,1);
                p->icon = 17;
            }
            virtual ~ParkingSpace();

            virtual void init();

            int variant;

            Sprite sprite;
            bool spriteNeedsUpdate;
            void updateSprite();
        };
    }
}
