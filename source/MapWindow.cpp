#include "Game.h"
#include "MapWindow.h"

using namespace OT;


void MapWindow::close()
{
	if (window) {
		window->RemoveReference();
		window->Close();
	}
	window = NULL;
}

void MapWindow::reload()
{
	close();
	window = game->gui.loadDocument("map.rml");
	if (window) window->Show();
}
