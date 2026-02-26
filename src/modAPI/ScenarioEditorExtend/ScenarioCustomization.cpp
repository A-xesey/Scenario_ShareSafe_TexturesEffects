#include "stdafx.h"
#include "ScenarioCustomization.h"
#include "ScenarioCustomizationItem.h"
#include <string>
#include <iostream>

using namespace Simulator;
using namespace Terrain;
using namespace App;

namespace {
	const float	GRID_START_X = 0.0f;
	const float	GRID_START_Y = 0.0f;
	const float	ITEM_MARGIN = 10.0f;//TBD
	const float	ITEM_WIDTH = 64.0f + ITEM_MARGIN;//TBD
	const float	ITEM_HEIGHT = 64.0f + ITEM_MARGIN;//TBD
}

ScenarioCustomization::ScenarioCustomization() : initialized(false), openedWinID(0)
{
}

ScenarioCustomization::~ScenarioCustomization()
{
}

void ScenarioCustomization::InitItems(IWindow* window)
{
	if (!initialized && window != nullptr)
	{
		Math::Rectangle areaWin = window->GetRealArea();
		float widthWin = areaWin.right - areaWin.left;
		int columns = (int)((widthWin + ITEM_MARGIN) / ITEM_WIDTH);
		if (columns < 1) columns = 1;

		vector<uint32_t> textures;
		PropManager.GetPropertyListIDs(id("ScenarioCustomizationTextures"), textures);
		if (!textures.empty())
		{
			int index = 0;
			for (const uint32_t& texture : textures)
			{
				PropertyListPtr propList;
				if (PropManager.GetPropertyList(texture, id("ScenarioCustomizationTextures"), propList))
				{
					ScenarioCustomizationItem* item = new ScenarioCustomizationItem();
					item->SetCustomizationAndImage(*propList.get(), this);
					item->SetParentWindow(window);

					int row = index / columns;
					int col = index % columns;
					float xPosition = GRID_START_X + (col * ITEM_WIDTH);
					float yPosition = GRID_START_Y + (row * ITEM_HEIGHT);

					IWindow* itemWin = item->FindWindowByID(id("PlanetCustomizationItem"));
					if (itemWin != nullptr)
					{
						Math::Rectangle areaItem = itemWin->GetArea();

						float width = areaItem.right - areaItem.left;
						float height = areaItem.bottom - areaItem.top;
						areaItem.left = xPosition;
						areaItem.top = yPosition;
						areaItem.right = xPosition + width;
						areaItem.bottom = yPosition + height;

						itemWin->SetArea(areaItem);
					}

					items.push_back(item);
					index++;
				}
			}
		}
	}
	initialized = true;
}

// For internal use, do not modify.
int ScenarioCustomization::AddRef()
{
	return DefaultRefCounted::AddRef();
}

// For internal use, do not modify.
int ScenarioCustomization::Release()
{
	return DefaultRefCounted::Release();
}

// You can extend this function to return any other types your class implements.
void* ScenarioCustomization::Cast(uint32_t type) const
{
	CLASS_CAST(Object);
	CLASS_CAST(IWinProc);
	CLASS_CAST(ScenarioCustomization);
	return nullptr;
}

// This method returns a combinations of values in UTFWin::EventFlags.
// The combination determines what types of events (messages) this window procedure receives.
// By default, it receives mouse/keyboard input and advanced messages.
int ScenarioCustomization::GetEventFlags() const
{
	return kEventFlagBasicInput | kEventFlagAdvanced;
}


unsigned long uintFromString(const std::u16string& i)
{
	unsigned long hexe = std::stoul(std::string(i.begin(), i.end()), nullptr, 16);
	return hexe;
}

// The method that receives the message. The first thing you should do is probably
// checking what kind of message was sent...
bool ScenarioCustomization::HandleUIMessage(IWindow* window, const Message& message)
{
	if (message.IsType(UTFWin::kMsgButtonClick))
	{ // move id("PlanetCustomizationTextureButton") to .h later
		if (message.source->GetControlID() == id("PlanetCustomizationTextureButton"))
		{
			if (openedWinID != id("PlanetCustomizationTextureButton"))
			{
				message.source->GetParent()->FindWindowByID(id("PlanetCustomizationPanel"))->SetVisible(true);
				message.source->GetParent()->FindWindowByID(id("PlanetCustomizationItems"))->SetVisible(true);
				openedWinID = id("PlanetCustomizationTextureButton");
			}
			else
			{
				message.source->GetParent()->FindWindowByID(id("PlanetCustomizationPanel"))->SetVisible(false);
				message.source->GetParent()->FindWindowByID(id("PlanetCustomizationItems"))->SetVisible(false);
				openedWinID = 0;
			}
		}
		else if (message.source->GetControlID() == id("PlanetCustomizationItem"))
		{
			cScenarioTerraformMode* scnTerrain = ScenarioMode.GetTerraformMode();
			if (scnTerrain != nullptr)
			{
				scnTerrain->StartHistoryEntry();
				eastl::string16 ItemDirectoryString = message.source->FindWindowByID(id("PlanetCustomizationItemDirectory"))->GetCaption();
				if (ItemDirectoryString.c_str() != nullptr)
				{
					ResourceKey key;
					auto groupID = ItemDirectoryString.find_first_of(u"!");
					auto typeID = ItemDirectoryString.find_last_of(u".");

					auto linstanceID = uintFromString(ItemDirectoryString.substr(groupID + 1, typeID - groupID - 1).c_str());
					auto lgroupID = uintFromString(ItemDirectoryString.substr(0, groupID).c_str());
					auto ltypeID = uintFromString(ItemDirectoryString.substr(typeID + 1, ItemDirectoryString.length() - typeID).c_str());

					key = { linstanceID, ltypeID, lgroupID };

					/// set texture
					//terrainThemeCliff
					scnTerrain->mpPropList->SetProperty(0x03b4f7c6, &Property().SetValueKey(key));
					//terrainThemeAboveDetail2
					scnTerrain->mpPropList->SetProperty(0x03B4F7C9, &Property().SetValueKey(key));
					//terrainThemeAboveDetailNoise
					scnTerrain->mpPropList->SetProperty(0x03b4f7ca, &Property().SetValueKey(key));
					//terrainThemeBelow
					scnTerrain->mpPropList->SetProperty(0x03b4f7cb, &Property().SetValueKey(key));
					//terrainThemeBeach2
					scnTerrain->mpPropList->SetProperty(0x03b4f7cd, &Property().SetValueKey(key));
					//update current textures
					//cTerrainStateMgr_UpdateFromDefinition
					CALL(
						Address(ModAPI::ChooseAddress(0xf902d0, 0xfbc100)),
						void,
						Args(cTerrainStateMgr*, PropertyList*),
						Args(scnTerrain->mpTerrainStateMgr, scnTerrain->mpPropList.get()));
				}
				scnTerrain->CommitHistoryEntry();
			}
			scnTerrain = nullptr;
		}
	}
	// Return true if the message was handled, and therefore no other window procedure should receive it.
	return false;
}