#include "../Game.h"
#include "../Math/Rand.h"
#include "Security.h"

using namespace OT;
using namespace OT::Item;


Security::~Security()
{
}

void Security::init()
{
    Item::init();

    sprite.SetImage(App->bitmaps["simtower/security"]);
    sprite.setOrigin(0, 24);
    sprite.setPosition(getPosition().x * 8, -getPosition().y * 36);
    addSprite(&sprite);
    spriteNeedsUpdate = false;

    updateSprite();
}

void Security::advance(double dt)
{
    Item::advance(dt); // Call base class advance if it ever does anything

    // Deduct operating costs (e.g., weekly, similar to Office rent collection timing)
    // SimTower was $10,000/month. If a game "month" is roughly 4 "weeks":
    if (game->time.checkHour(0) && game->time.day == 0) { // Day 0 is the start of the week
        game->transferFunds(-kWeeklyOperatingCost, "Security Office Upkeep");
    }

    if (spriteNeedsUpdate) {
        updateSprite();
    }
}

void Security::encodeXML(tinyxml2::XMLPrinter & xml)
{
    Item::encodeXML(xml);
    xml.PushAttribute("variant", variant);
}

void Security::decodeXML(tinyxml2::XMLElement & xml)
{
    Item::decodeXML(xml);
    variant = xml.IntAttribute("variant"); // Default to 0 if attribute not found
    spriteNeedsUpdate = true; // Ensure sprite updates on load
}

void Security::updateSprite()
{
    spriteNeedsUpdate = false;
    int index = 0; // Assuming the security office sprite is not animated frame by frame (column index)
    // Variant will select the row in the sprite sheet.
    // e.g., row 0 for normal, row 1 for alert.
    sprite.setTextureRect(sf::IntRect(index*128, variant*24, 128, 24));
}

void Security::setAlert(bool isAlerting) {
    int newVariant = isAlerting ? 1 : 0;
    if (variant != newVariant) {
        variant = newVariant;
        spriteNeedsUpdate = true;
    }
}