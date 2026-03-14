/* Copyright (c) 2012-2015 Fabian Schuiki */
#include "../Application.h"
#include "Cathedral.h"

using namespace OT;
using namespace OT::Item;

Cathedral::~Cathedral()
{
}

void Cathedral::init()
{
	Item::init();
	/* Placeholder: use skylobby asset until SimTower cathedral art is added. */
	sprite.SetImage(App->bitmaps["simtower/lobby/sky"]);
	sprite.setOrigin(0, 36);
	addSprite(&sprite);
	updateSprite();
}

void Cathedral::updateSprite()
{
	sprite.setPosition(getPositionPixels().x, -getPositionPixels().y);
	sprite.setTextureRect(sf::IntRect(56, 0, size.x * 8, size.y * 36));
}

void Cathedral::encodeXML(tinyxml2::XMLPrinter & xml)
{
	Item::encodeXML(xml);
}

void Cathedral::decodeXML(tinyxml2::XMLElement & xml)
{
	Item::decodeXML(xml);
	updateSprite();
}
