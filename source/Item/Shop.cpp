#include "../Game.h"
#include "../Math/Rand.h"
#include "Shop.h"

using namespace OT;
using namespace OT::Item;


Shop::~Shop()
{
}

void Shop::init()
{
    Item::init();

    sprite.SetImage(App->bitmaps["simtower/shops"]);
    sprite.setOrigin(0, 24);
    sprite.setPosition(getPosition().x * 8, -getPosition().y * 36);
    addSprite(&sprite);
    spriteNeedsUpdate = false;

    updateSprite();
}

void Shop::updateSprite()
{
    spriteNeedsUpdate = false;
    int index = 0;
    sprite.setTextureRect(sf::IntRect(index*128, variant*24, 128, 24));
}