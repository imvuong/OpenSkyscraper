#include "../Game.h"
#include "../Math/Rand.h"
#include "ParkingSpace.h"

using namespace OT;
using namespace OT::Item;


ParkingSpace::~ParkingSpace()
{
}

void ParkingSpace::init()
{
    Item::init();

    sprite.SetImage(App->bitmaps["simtower/parking/space"]);
    sprite.setOrigin(0, 24);
    sprite.setPosition(getPosition().x * 8, -getPosition().y * 36);
    addSprite(&sprite);
    spriteNeedsUpdate = false;

    updateSprite();
}

void ParkingSpace::updateSprite()
{
    spriteNeedsUpdate = false;
    int index = 0;
    sprite.setTextureRect(sf::IntRect(index*128, variant*24, 128, 24));
}