#pragma once
#include <Rocket/Core/ElementDocument.h>
#include "GameObject.h"

namespace OT {

	class MapWindow : public GameObject
	{
	public:
		MapWindow(Game * game) : GameObject(game) {
			window = NULL;
		}
		~MapWindow() { close(); }

		void close();
		void reload();

		Rocket::Core::ElementDocument * window;
	};
}
