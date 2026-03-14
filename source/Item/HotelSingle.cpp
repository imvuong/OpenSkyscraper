#include "../Game.h"
#include "../Math/Rand.h"
#include "../Time.h"
#include "HotelSingle.h"

using namespace OT;
using namespace OT::Item;


HotelSingle::~HotelSingle()
{
}

void HotelSingle::init()
{
    Item::init();

    state = kEmpty;
    dirtySince = 0;

    sprite.SetImage(App->bitmaps["simtower/single"]);
    sprite.setOrigin(0, 24);
    sprite.setPosition(getPosition().x * 8, -getPosition().y * 36);
    addSprite(&sprite);
    spriteNeedsUpdate = false;

    updateSprite();
}

void HotelSingle::encodeXML(tinyxml2::XMLPrinter & xml)
{
    Item::encodeXML(xml);
    xml.PushAttribute("state", (int)state);
    if (state == kDirty) xml.PushAttribute("dirtySince", dirtySince);
    xml.PushAttribute("variant", variant);
}

void HotelSingle::decodeXML(tinyxml2::XMLElement & xml)
{
    Item::decodeXML(xml);
    state = (State)xml.IntAttribute("state");
    dirtySince = xml.DoubleAttribute("dirtySince");
    variant = xml.IntAttribute("variant");
    updateSprite();
}

void HotelSingle::advance(double dt)
{
    const double checkInHour = 17.0;
    const double checkoutHour = 10.0;
    const double cleanDuration = 2.0 * Time::hourToAbsolute(1);

    if (state == kEmpty && !lobbyRoute.empty() && game->time.checkHour(checkInHour)) {
        state = kOccupied;
        spriteNeedsUpdate = true;
    }
    if (state == kOccupied && game->time.checkHour(checkoutHour)) {
        state = kDirty;
        dirtySince = game->time.absolute;
        spriteNeedsUpdate = true;
    }
    if (state == kDirty) {
        bool hasHousekeeping = game->itemsByType.find("housekeeping") != game->itemsByType.end() && !game->itemsByType["housekeeping"].empty();
        if (hasHousekeeping && (game->time.absolute - dirtySince) >= cleanDuration) {
            state = kEmpty;
            dirtySince = 0;
            spriteNeedsUpdate = true;
        }
    }

    if (spriteNeedsUpdate) updateSprite();
}

void HotelSingle::updateSprite()
{
    spriteNeedsUpdate = false;
    int index = (state == kOccupied ? 1 : (state == kDirty ? 2 : 0));
    sprite.setTextureRect(sf::IntRect(index*128, variant*24, 128, 24));
}