#include "stdafx.h"
#include "ScenarioCustomization.h"
#include "ScenarioCustomizationItem.h"
#include <string>
#include <iostream>

using namespace Simulator;
using namespace Terrain;
using namespace App;

bool isInit = false;

void ScenarioCustomization::InitItems(UTFWin::IWindow* window)
{
	if (!isInit)
	{
		vector<uint32_t> textures;
		PropManager.GetPropertyListIDs(id("ScenarioCustomizationTextures"), textures);
		if (textures.size() != 0)
		{
			for (const uint32_t& texture : textures)
			{
				PropertyListPtr propList;
				PropManager.GetPropertyList(texture, id("ScenarioCustomizationTextures"), propList);
				if (propList != nullptr)
				{
					ScenarioCustomizationItem* item = new ScenarioCustomizationItem();
					item->SetCustomizationAndImage(*propList.get());
					item->SetParentWindow(window);
					item->FindWindowByID(id("PlanetCustomizationItem"))->AddWinProc(this);
				}
			}
		}
	}
	isInit = true;
}

ScenarioCustomization::ScenarioCustomization()
{
}

ScenarioCustomization::~ScenarioCustomization()
{
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
	{
		if (message.source->GetControlID() == id("PlanetCustomizationTextureButton"))
		{
			message.source->GetParent()->FindWindowByID(id("PlanetCustomizationPanel"))->SetVisible(true);
			message.source->GetParent()->FindWindowByID(id("PlanetCustomizationItems"))->SetVisible(true);
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