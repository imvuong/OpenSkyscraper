#pragma once
#include "../Sprite.h"
#include "../Game.h" // Required for Game::time access in advance
#include "Item.h"
#include <set>

namespace OT {
    namespace Item {
        class Security : public Item
        {
        public:
            OT_ITEM_CONSTRUCTOR(Security);
            OT_ITEM_PROTOTYPE(Security) {
                p->id = "security";
                p->name = "Security";
                p->price = 100000;
                p->size = int2(16,1);
                p->icon = 21;
                p->minRating = 1;
            }
            virtual ~Security();

            virtual void init() override;
            virtual void advance(double dt) override;

            virtual void encodeXML(tinyxml2::XMLPrinter & xml) override;
            virtual void decodeXML(tinyxml2::XMLElement & xml) override;

            int variant;

            Sprite sprite;
            bool spriteNeedsUpdate;
            void updateSprite();

            void setAlert(bool isAlerting);

            static const int kMonthlySimTowerCost = 10000;
            // Assuming 4 weeks per month for a weekly deduction if game time is weekly based
            static const int kWeeklyOperatingCost = kMonthlySimTowerCost / 4;
        };
    }
}
