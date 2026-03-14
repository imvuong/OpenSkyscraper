#include <cassert>
#include <cstdio>
#include <Rocket/Core/Element.h>
#include "Game.h"
#include "ToolboxWindow.h"
#include "Item/Factory.h"

using namespace OT;


void ToolboxWindow::close()
{
	buttons.clear();
	
	if (window) {
		window->RemoveReference();
		window->Close();
	}
	window = NULL;
}

void ToolboxWindow::reload()
{
	close();
	
	window = game->gui.loadDocument("toolbox.rml");
	assert(window);
	window->Show();
	
	Rocket::Core::ElementList tmp;
	window->GetElementsByTagName(tmp, "button");
	for (int i = 0; i < tmp.size(); i++) {
		tmp[i]->AddEventListener("click", this);
		if (tmp[i]->IsClassSet("tool")) buttons.insert(tmp[i]);
	}
	
	//Add the item buttons.
	for (int i = 0; i < game->itemFactory.prototypes.size(); i++) {
		Item::AbstractPrototype * prototype = game->itemFactory.prototypes[i];
		
		char id[128];
		snprintf(id, 128, "item-%s", prototype->id.c_str());
		
		char style[512];
		snprintf(style, 512, "button#%s { background-image-s: %ipx %ipx; }", id, prototype->icon*32, prototype->icon*32+32);
		LOG(DEBUG, "style for %s: %s", prototype->name.c_str(), style);
		
		Rocket::Core::StyleSheet * sheet = Rocket::Core::Factory::InstanceStyleSheetString(style);
		assert(sheet && "unable to instantiate stylesheet");
		window->SetStyleSheet(window->GetStyleSheet()->CombineStyleSheet(sheet));
		sheet->RemoveReference();
		
		Rocket::Core::XMLAttributes attributes;
		attributes.Set("class", "item");
		attributes.Set("id", id);
		
		Rocket::Core::Element * button = Rocket::Core::Factory::InstanceElement(NULL, "button", "button", attributes);
		assert(button && "unable to instantiate button");
		
		button->AddEventListener("click", this);
		window->AppendChild(button);
		buttons.insert(button);
		
		button->RemoveReference();
	}
	
	updateSpeed();
	updateTool();
	updateLockedButtons();
}

void ToolboxWindow::ProcessEvent(Rocket::Core::Event & event)
{
	LOG(DEBUG, "%s received", event.GetType().CString());
	Rocket::Core::Element * element = event.GetCurrentElement();
	if (buttons.count(element)) {
		if (element->IsClassSet("locked")) {
			Rocket::Core::String id = element->GetId();
			if (id.Find("item-") == 0) {
				std::string prototypeId(id.CString() + 5);
				Item::AbstractPrototype * proto = game->itemFactory.prototypesById[prototypeId];
				if (proto) {
					char msg[64];
					snprintf(msg, sizeof(msg), "%s unlocks at %d stars.", proto->name.c_str(), proto->minRating + 1);
					game->timeWindow.showMessage(msg);
				}
			}
			event.StopPropagation();
			return;
		}
		game->selectTool(element->GetId().CString());
		event.StopPropagation();
	} else if (element->IsClassSet("speed")) {
		int speed;
		sscanf(element->GetId().CString(), "speed%i", &speed);
		game->setSpeedMode(speed);
		event.StopPropagation();
	}
}

void ToolboxWindow::updateSpeed()
{
	for (int i = 0; i < 4; i++) {
		char c[32];
		snprintf(c, 32, "speed%i", i);
		Rocket::Core::Element * button = window->GetElementById(c);
		assert(button);
		button->SetClass("selected", game->speedMode == i);
	}
}

void ToolboxWindow::updateTool()
{
	for (ElementSet::iterator b = buttons.begin(); b != buttons.end(); b++) {
		(*b)->SetClass("selected", (*b)->GetId() == game->selectedTool.c_str());
	}
}

void ToolboxWindow::updateLockedButtons()
{
	for (ElementSet::iterator b = buttons.begin(); b != buttons.end(); b++) {
		Rocket::Core::String id = (*b)->GetId();
		if (id.Find("item-") != 0) continue;
		std::string prototypeId(id.CString() + 5);
		std::map<std::string, Item::AbstractPrototype *>::iterator it = game->itemFactory.prototypesById.find(prototypeId);
		if (it == game->itemFactory.prototypesById.end()) continue;
		bool locked = (it->second->minRating > game->rating);
		(*b)->SetClass("locked", locked);
	}
}
