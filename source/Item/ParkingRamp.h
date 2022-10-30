#pragma once
#include "../Sprite.h"
#include "Item.h"
#include <set>

namespace OT {
    namespace Item {
        class ParkingRamp : public Item
        {
        public:
            OT_ITEM_CONSTRUCTOR(ParkingRamp);
            OT_ITEM_PROTOTYPE(ParkingRamp) {
                p->id = "ramp";
                p->name = "Parking Ramp";
                p->price = 100000;
                p->size = int2(16,1);
                p->icon = 16;
            }
            virtual ~ParkingRamp();

            virtual void init();

            int variant;

            Sprite sprite;
            bool spriteNeedsUpdate;
            void updateSprite();
        };
    }
}
